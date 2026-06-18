// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "InteractibleActor.generated.h"

UCLASS()
class EGGTEST_API AInteractibleActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractibleActor();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UStaticMeshComponent* InteractMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(class APawn* InteractPlayer) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnInteract();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
