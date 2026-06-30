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

	if (bCanInteract && bAutoSimulatePhyisicsOnDrop)
		InteractMesh->SetSimulatePhysics(true);
	else if(!bCanInteract)
	{
		if (bAutoSimulatePhyisicsOnDrop)
			InteractMesh->SetSimulatePhysics(false);

		if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
		{
			AttachToComponent(MyOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ActorAttachSocketName);
		}
	}
}
