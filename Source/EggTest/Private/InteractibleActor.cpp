// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleActor.h"
#include "Components/BoxComponent.h"
#include "./MyPlayerCharacter.h"
#include "Components/WidgetComponent.h"

// Sets default values
AInteractibleActor::AInteractibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	InteractMesh->SetupAttachment(RootComponent);

	InteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractArea"));
	InteractArea->SetupAttachment(RootComponent);
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

void AInteractibleActor::OnInteract_Implementation(APawn* InteractingPlayer)
{

}

void AInteractibleActor::Interact_Implementation(class APawn* InteractPlayer)
{
	// Only run OnInteract on the server
	if (HasAuthority())
	{
		OnInteract(InteractPlayer);
	}
}

void AInteractibleActor::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
	{
		if (Player->IsLocallyControlled())
		{
			Player->AvailableInteractingActor = this;
			WidgetComp->SetVisibility(true);
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

