// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EggActor.generated.h"

class AMyPlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class EGGTEST_API AEggActor : public AActor
{
	GENERATED_BODY()

public:
	AEggActor();
	virtual void Tick(float DeltaTime) override;

	// Owner and trail index
	UPROPERTY(Replicated)
	AMyPlayerCharacter* OwnerPlayer;

	UPROPERTY(Replicated)
	int32 SnakeIndex;

	UPROPERTY(EditAnywhere)
	int32  ScoreAmount = 50;

	// Launch functions
	UFUNCTION(BlueprintCallable)
	void LaunchEgg(float LaunchStrength, float UpwardStrength, AMyPlayerCharacter* InstOwner);

	UFUNCTION(BlueprintCallable)
	void DetachFromPlayer();

	UFUNCTION(Server, Reliable)
	void ServerSetEggCollision();

public:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	// Train settings
	UPROPERTY(EditDefaultsOnly, Category = "Train")
	float SegmentDistance = 150.f;

	UFUNCTION()
	void OnEggOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnEggHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FVector LastLocation;
	FVector GetFollowPosition() const;
};
