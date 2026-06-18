// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleActor.h"
#include "DoorActor.generated.h"

UCLASS()
class EGGTEST_API ADoorActor : public AInteractibleActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();

	UPROPERTY(ReplicatedUsing = OnRep_DoorOpen, EditAnywhere, BlueprintReadWrite)
	uint8 bDoorOpen : 1;

	UPROPERTY(Replicated)
	float Dir = 0;

	UFUNCTION()
	void OnRep_DoorOpen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(class APawn* InteractPlayer) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintNativeEvent)
	void OnDoorToggled(bool bOpen, int32 direction);
};
