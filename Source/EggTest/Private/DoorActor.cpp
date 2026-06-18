// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorActor.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorActor::Interact_Implementation(class APawn* InteractPlayer)
{
	if (HasAuthority())
	{
		bDoorOpen = !bDoorOpen;
		Dir = FVector::DotProduct(GetActorForwardVector(), InteractPlayer->GetActorForwardVector());

		OnDoorToggled(bDoorOpen, Dir > 0 ? 1 : -1);
	}
}

void ADoorActor::OnRep_DoorOpen()
{
	OnDoorToggled(bDoorOpen, Dir > 0 ? 1 : -1);
}

void ADoorActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoorActor, bDoorOpen);
	DOREPLIFETIME(ADoorActor, Dir);
}

void ADoorActor::OnDoorToggled_Implementation(bool bOpen, int32 direction)
{

}
