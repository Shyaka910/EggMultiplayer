#include "MyPlayerCharacter.h"
#include "EggActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "CubeActor.h"
#include <MyGameState.h>
#include <MyPlayerState.h>
#include "MyGameMode.h"
#include "MultiplayerSessionSubsystem.h"
#include "Components/CombatComponent.h"
#include "Components/CharacterAbilitySystemComponent.h"
#include "Gameplay/CharacterAttributeSet.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	PlayerAbilityComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilityComp"));

	MyCharacterAttribute = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("MyAttributes"));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this, &AMyPlayerCharacter::OnPlayerBeginOverlap);

	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(
		this, &AMyPlayerCharacter::OnPlayerEndOverlap);
}

class UAbilitySystemComponent* AMyPlayerCharacter::GetAbilitySystemComponent() const
{
	return PlayerAbilityComponent;
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Eggs handle their own movement
}

void AMyPlayerCharacter::TravelToMap(FName MapName , bool bBacktoLobbyMap)
{
	if (!HasAuthority()) return;

	FString ServerMapName = FString::Printf(TEXT("/Game/ThirdPerson/Maps/%s?listen"), *MapName.ToString());
	GetWorld()->ServerTravel(ServerMapName);

	if (UMultiplayerSessionSubsystem* SessionSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionSubsystem>())
	{
		if (!bBacktoLobbyMap)
			SessionSubsystem->StartSession();
		else
			SessionSubsystem->EndGameSession();
	}
}

void AMyPlayerCharacter::RegisterEgg(AEggActor* NewEgg)
{
	if (!HasAuthority()) return;

	if (!NewEgg) return;

	NewEgg->SetOwner(this);
	EggTrail.Add(NewEgg);
	NewEgg->OwnerPlayer = this;
	NewEgg->SnakeIndex = EggTrail.Num(); // 1-based index

	// Add score to the player who picked an egg

	if (AMyGameState* GS = Cast<AMyGameState>(GetWorld()->GetGameState()))
	{
		if (AMyPlayerState* PS = GetPlayerState<AMyPlayerState>())
		{
			GS->AddScoreToPlayer(PS, NewEgg->ScoreAmount);
		}
	}
}

void AMyPlayerCharacter::ChangePickedCube()
{
	if (EquippedCube)
	{
		EquippedCube->UpdateCubeColor();
	}
}

// Launch your first egg
void AMyPlayerCharacter::LaunchFirstEgg(float LaunchStrength,  float UpwardStrength)
{
	if (!HasAuthority())
	{
		ServerLaunchFirstEgg(LaunchStrength, UpwardStrength);
		return;
	}

	if (EggTrail.Num() == 0) return;

	// Launch the first egg
	AEggActor* EggToLaunch = EggTrail[0];
	if (EggToLaunch)
	{
		EggToLaunch->LaunchEgg(LaunchStrength, UpwardStrength, this);
	}

	// Update SnakeIndex for remaining eggs
	for (int32 i = 0; i < EggTrail.Num(); i++)
	{
		if (EggTrail[i])
		{
			EggTrail[i]->SnakeIndex = i + 1; // 1-based
		}
	}
}

void AMyPlayerCharacter::ServerLaunchFirstEgg_Implementation(float LaunchStrength, float UpwardStrength)
{
	LaunchFirstEgg(LaunchStrength, UpwardStrength);
}

// Launch all other players' eggs
void AMyPlayerCharacter::LaunchOtherPlayersEggs(float LaunchStrength, float UpwardStrength)
{
	if (!HasAuthority())
	{
		ServerLaunchOtherPlayersEggs(LaunchStrength, UpwardStrength);
		return;
	}
	if (OtherPlayerEggToThrow)
	{
		AMyPlayerCharacter* OwnerOfEgg = OtherPlayerEggToThrow->OwnerPlayer;

		OtherPlayerEggToThrow->LaunchEgg(LaunchStrength, UpwardStrength, this);

		// Update SnakeIndex for other player remaining eggs
		if (!OwnerOfEgg) return;
		for (int32 i = 0; i < OwnerOfEgg->EggTrail.Num(); i++)
		{
			if (OwnerOfEgg->EggTrail[i])
			{
				OwnerOfEgg->EggTrail[i]->SnakeIndex = i + 1; // 1-based
			}
		}
	}
	
}

void AMyPlayerCharacter::ServerLaunchOtherPlayersEggs_Implementation(float LaunchStrength, float UpwardStrength)
{
	LaunchOtherPlayersEggs(LaunchStrength, UpwardStrength);
}

void AMyPlayerCharacter::ServerSetOverlappedEgg_Implementation(AActor* OtherActor)
{
	if (AEggActor* OverlappedEgg = Cast<AEggActor>(OtherActor))
	{
		if (OverlappedEgg->OwnerPlayer != this && OverlappedEgg->OwnerPlayer != nullptr)
		{
			OverlappedEgg->ProjectileMovement->SetUpdatedComponent(RootComponent);
			OverlappedEgg->ProjectileMovement->Deactivate();

			OtherPlayerEggToThrow = OverlappedEgg;
		}
	}
}

void AMyPlayerCharacter::ServerRemoveOverlappedEgg_Implementation(AActor* OtherActor)
{
	if (AEggActor* OverlappedEgg = Cast<AEggActor>(OtherActor))
	{
		if (OtherPlayerEggToThrow)
		{

			OtherPlayerEggToThrow->CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			OtherPlayerEggToThrow->CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
			OtherPlayerEggToThrow->CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

			// Launch the first egg
			AEggActor* EggToLaunch = OtherPlayerEggToThrow;
			EggToLaunch->LaunchEgg(1500.f, 0.3f, this);

			// Update SnakeIndex for remaining eggs
			for (int32 i = 0; i < EggTrail.Num(); i++)
			{
				if (EggTrail[i])
				{
					EggTrail[i]->SnakeIndex = i + 1; // 1-based
				}
			}

			// clear the egg that was stolen
			OtherPlayerEggToThrow = nullptr;
		}
	}
}

void AMyPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerCharacter, EggTrail);
	DOREPLIFETIME(AMyPlayerCharacter, OtherPlayerEggToThrow);
	DOREPLIFETIME(AMyPlayerCharacter, EquippedCube);
}


float AMyPlayerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Health -= Damage;
		CombatComponent->OnHealthUpdated.Broadcast(CombatComponent->Health);
		// TO-DO Use Curve to Descrease UI Health;

		return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	return 0.f;
}

void AMyPlayerCharacter::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		ServerSetOverlappedEgg(OtherActor);
	else
	{
		// reset collision
		if (AEggActor* OverlappedEgg = Cast<AEggActor>(OtherActor))
		{
			OverlappedEgg->ProjectileMovement->Deactivate();
		}
	}
}

void AMyPlayerCharacter::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
		ServerRemoveOverlappedEgg(OtherActor);
}
