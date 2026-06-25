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
	bool bIsAuto = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float FireRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float DamageRate = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FName WeaponAttachSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FName WeaponMuzzleSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class UAnimMontage* AimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnWeaponFireHit(class ACharacter* HitPlayer, AActor* BulletActor, const FHitResult& HitRes);

	virtual void Interact_Implementation(class APawn* InteractPlayer) override;

	UFUNCTION(BlueprintCallable)
	virtual void Fire();

	void StartFire();

	UFUNCTION(BlueprintCallable)
	virtual void EndFire();

	UFUNCTION(Server, Reliable)
	void ServerFire(FVector SocketLocation, FRotator SocketRotation, FVector CameraLoc, FVector AimLoc);

	UFUNCTION(NetMulticast, Reliable)
	void Mutlicast_PlayCosmetic(FVector SocketLocation, FRotator SocketRotation);

protected:

	virtual void Tick(float DeltaTime) override;

private:

	float currentFireTimer = 0;
	bool bIsFiring = false;
	bool bFirePressed = false;
	
};
