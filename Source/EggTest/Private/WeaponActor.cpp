// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "MyPlayerCharacter.h"

void AWeaponActor::Interact_Implementation(class APawn* InteractPlayer)
{
	// this function is called only on the server
	Super::Interact_Implementation(InteractPlayer);

	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(InteractPlayer))
	{
		if (bCanInteract)
			Player->EquipeWeapon(this);
	}
}
