// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleActor.h"
#include "Components/BoxComponent.h"
#include "./MyPlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AInteractibleActor::AInteractibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = InteractMesh;
	InteractMesh->SetupAttachment(RootComponent);

	InteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractArea"));
	InteractArea->SetupAttachment(InteractMesh);
	InteractArea->SetGenerateOverlapEvents(true);

	InteractArea->OnComponentBeginOverlap.AddDynamic(this, &AInteractibleActor::OnActorBeginOverlap);
	InteractArea->OnComponentEndOverlap.AddDynamic(this, &AInteractibleActor::OnActorEndOverlap);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	WidgetComp->SetupAttachment(InteractMesh);
}

// Called when the game starts or when spawned
void AInteractibleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractibleActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractibleActor, bCanInteract);
}

void AInteractibleActor::Interact_Implementation(class APawn* InteractPlayer)
{
	// Only run OnInteract on the server
	if (HasAuthority() && Execute_CanInteract(this))
	{
		NetMutlicast_OnInteract(InteractPlayer);
	}
}

bool AInteractibleActor::CanInteract_Implementation()
{
	return bCanInteract;
}

void AInteractibleActor::SetCanInteract_Implementation(bool bInteract /*= true*/)
{
	if (HasAuthority())
		bCanInteract = bInteract;
}

void AInteractibleActor::NetMutlicast_OnInteract_Implementation(APawn* InteractingPlayer)
{
	OnInteract(InteractingPlayer);
}

void AInteractibleActor::OnInteract_Implementation(APawn* InteractingPlayer)
{

}

void AInteractibleActor::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
	{
		if (Player->IsLocallyControlled())
		{
			if (bCanInteract)
			{
				Player->AvailableInteractingActor = this;
				WidgetComp->SetVisibility(true);
			}
			else
			{
				Player->AvailableInteractingActor = nullptr;
				WidgetComp->SetVisibility(false);
			}
		}
	}
}

void AInteractibleActor::OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
	{
		if (Player->IsLocallyControlled())
		{
			Player->AvailableInteractingActor = nullptr;
			WidgetComp->SetVisibility(false);
		}
	}
}

// Called every frame
void AInteractibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractibleActor::OnRep_CanInteract()
{

}

