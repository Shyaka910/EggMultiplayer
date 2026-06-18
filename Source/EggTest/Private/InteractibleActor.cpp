// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleActor.h"

// Sets default values
AInteractibleActor::AInteractibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	InteractMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AInteractibleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractibleActor::OnInteract_Implementation()
{

}

void AInteractibleActor::Interact_Implementation(class APawn* InteractPlayer)
{
	// Only run OnInteract on the server
	if (HasAuthority())
	{
		OnInteract();
	}
}

// Called every frame
void AInteractibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

