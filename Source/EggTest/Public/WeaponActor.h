// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleActor.h"
#include "WeaponActor.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API AWeaponActor : public AInteractibleActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FName WeaponAttachSocketName = "";

	virtual void Interact_Implementation(class APawn* InteractPlayer) override;
	
};
