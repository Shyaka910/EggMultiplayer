#include "EggActor.h"
#include "MyPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEggActor::AEggActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	CollisionComp->SetSphereRadius(50.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEggActor::OnEggOverlap);
	CollisionComp->OnComponentHit.AddDynamic(this, &AEggActor::OnEggHit);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->bAutoActivate = false; // don't move until launched
	ProjectileMovement->SetIsReplicated(true);

	SegmentDistance = 150.f;
}

void AEggActor::BeginPlay()
{
	Super::BeginPlay();
	LastLocation = GetActorLocation();
}

void AEggActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerPlayer) return;

	// Stop follow if launched
	if (ProjectileMovement && ProjectileMovement->IsActive())
		return;

	FVector Target = GetFollowPosition();

	FVector ToTarget = Target - GetActorLocation();
	float DistToTarget = ToTarget.Size();
	if (DistToTarget > KINDA_SMALL_NUMBER)
	{
		FVector Direction = ToTarget / DistToTarget;
		FVector DesiredLocation = Target - Direction * SegmentDistance;
		FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime, 20.f);
		SetActorLocation(NewLocation);
		LastLocation = NewLocation;
	}
}


FVector AEggActor::GetFollowPosition() const
{
	if (!OwnerPlayer) return GetActorLocation();

	if (SnakeIndex == 1)
	{
		return OwnerPlayer->GetActorLocation();
	}

	if (OwnerPlayer->EggTrail.IsValidIndex(SnakeIndex - 2))
	{
		AEggActor* PrevEgg = OwnerPlayer->EggTrail[SnakeIndex - 2];
		if (PrevEgg)
		{
			return PrevEgg->LastLocation;
		}
	}

	return OwnerPlayer->GetActorLocation();
}

void AEggActor::DetachFromPlayer()
{
	if (!HasAuthority()) return;

	if (OwnerPlayer)
	{
		OwnerPlayer->EggTrail.Remove(this); // safety
	}

	OwnerPlayer = nullptr;
	SetOwner(nullptr);
	SnakeIndex = 0;

	if (ProjectileMovement)
	{
		ProjectileMovement->SetUpdatedComponent(RootComponent);
		ProjectileMovement->SetActive(true);
	}

	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
		CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void AEggActor::ServerSetEggCollision_Implementation()
{
	
}

void AEggActor::LaunchEgg(float LaunchStrength, float UpwardStrength, AMyPlayerCharacter* InstOwner)
{
	// Calculate launch direction: forward + upward

	if (!ProjectileMovement) return;


	FVector Forward = InstOwner ? InstOwner->GetController()->GetControlRotation().Vector() : FVector::ForwardVector;
	FVector LaunchDir = Forward + FVector::UpVector * UpwardStrength;
	LaunchDir.Normalize();

	// Remove from trail first
	DetachFromPlayer();

	SetOwner(this);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	ProjectileMovement->Velocity = LaunchDir * LaunchStrength;
	ProjectileMovement->Activate(true);
}


void AEggActor::OnEggOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor->HasAuthority()) return;

	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
	if (!Player) return;

	Player->RegisterEgg(this);

	if (CollisionComp)
	{
		ProjectileMovement->Deactivate();

		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
		CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void AEggActor::OnEggHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMovement->SetActive(true);
}

void AEggActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEggActor, OwnerPlayer);
	DOREPLIFETIME(AEggActor, SnakeIndex);
}
