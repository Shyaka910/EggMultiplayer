// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippableActor.h"
#include "WeaponActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponHitTarget, AActor*, TargetActor, bool, IsHeadShoot);

/**
 * 
 */
UCLASS()
class EGGTEST_API AWeaponActor : public AEquippableActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bIsAuto = false;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon)
	int32 CurrentMaganizeSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 MaxMaganizeSize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Weapon)
	int32 CurrentBulletsCount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 MaxBulletsCount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float FireRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float DamageRate = 20.f;

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
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon)
	bool bCanShoot;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponHitTarget OnWeaponHitTarget;

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnWeaponFireHit(class ACharacter* HitPlayer, AActor* BulletActor, const FHitResult& HitRes);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponFired();

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

	UFUNCTION(BlueprintCallable)
	void AddBullets(int32 BulletNum);

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_CanInteract() override;

private:

	float currentFireTimer = 0;
	bool bIsFiring = false;
	bool bFirePressed = false;
};
