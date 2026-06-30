// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleActor.h"
#include "EquippableActor.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API AEquippableActor : public AInteractibleActor
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equippable)
	FName ActorAttachSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equippable)
	bool bAutoSimulatePhyisicsOnDrop = true;

protected:

	virtual void OnInteract_Implementation(class APawn* InteractPlayer) override;

	virtual void OnRep_CanInteract() override;
	
};
