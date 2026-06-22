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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UBoxComponent* InteractArea;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UWidgetComponent* WidgetComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact_Implementation(class APawn* InteractPlayer) override;

	virtual bool CanInteract_Implementation() override;

	virtual void SetCanInteract_Implementation(bool bInteract = true) override;

	UFUNCTION(NetMulticast, Unreliable)
	virtual void NetMutlicast_OnInteract(APawn* InteractingPlayer);

	/** This event gets called on all clients once any player interact with this actor */
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(APawn* InteractingPlayer);

	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(Replicated)
	bool bCanInteract = true;

};
