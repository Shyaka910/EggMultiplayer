// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableActor.h"
#include "MyPlayerCharacter.h"

void AEquippableActor::OnInteract_Implementation(class APawn* InteractPlayer)
{
	Super::OnInteract_Implementation(InteractPlayer);

	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(InteractPlayer))
	{
		if (bCanInteract)
			Player->EquipeActor(this);
	}
}

void AEquippableActor::OnRep_CanInteract()
{
	Super::OnRep_CanInteract();

	if (bCanInteract)
		InteractMesh->SetSimulatePhysics(true);
	else
	{
		InteractMesh->SetSimulatePhysics(false);

		if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
		{
			AttachToComponent(MyOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ActorAttachSocketName);
		}
	}
}
