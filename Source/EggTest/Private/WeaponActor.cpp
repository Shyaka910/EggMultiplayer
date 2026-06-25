// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "MyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void AWeaponActor::Interact_Implementation(class APawn* InteractPlayer)
{
	// this function is called only on the server
	Super::Interact_Implementation(InteractPlayer);

	if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(InteractPlayer))
	{
		if (bCanInteract)
			Player->EquipeWeapon(this);
	}
}

void AWeaponActor::Fire()
{
	bFirePressed = true;
	if (!bIsFiring) StartFire();
}

void AWeaponActor::StartFire()
{
	if (currentFireTimer <= 0.f)
	{
		bIsFiring = true;
	}
	else
	{
		currentFireTimer = 0;
		return;
	}

	APawn* OwnerChar = Cast<APawn>(GetOwner());
	if (!OwnerChar && !OwnerChar->IsLocallyControlled()) return;

	FVector CameraLoc;
	FRotator CameraRot;
	OwnerChar->GetController()->GetPlayerViewPoint(CameraLoc, CameraRot);

	FVector AimPoint = CameraLoc + CameraRot.Vector() * 100000.f;

	// Optional trace to get actual hit point
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLoc,
		AimPoint,
		ECC_Visibility))
	{
		AimPoint = Hit.ImpactPoint;
	}

	FVector MuzzleLoc = InteractMesh->GetSocketLocation(WeaponMuzzleSocketName);

	FRotator SpawnRot = (AimPoint - MuzzleLoc).Rotation();

	ServerFire(MuzzleLoc, SpawnRot, CameraLoc, AimPoint);

	if (OwnerChar)
	{
		if (OwnerChar->IsLocallyControlled())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleLoc,
				SpawnRot);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetActorLocation());

			UGameplayStatics::PlayWorldCameraShake(GetWorld(), FireCameraShake, GetActorLocation(),
				300.f, 500.f);

			if (ACharacter* Char = Cast<ACharacter>(OwnerChar))
			{
				if (!Char->HasAuthority()) Char->GetMesh()->GetAnimInstance()->Montage_Play(AimMontage);
				Char->GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Shoot"));
			}
		}
	}
}

void AWeaponActor::EndFire()
{
	if (currentFireTimer <= 0.f && bIsFiring)
		bIsFiring = false;
	bFirePressed = false;
}

void AWeaponActor::ServerFire_Implementation(FVector SocketLocation, FRotator SocketRotation, FVector CameraLoc, FVector AimLoc)
{
	APawn* OwnerChar = Cast<APawn>(GetOwner());

	if (OwnerChar)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                 // Weapon or Character
		SpawnParams.Instigator = OwnerChar; // Usually the firing Pawn

		AActor* BulletActor = GetWorld()->SpawnActor<AActor>(
			BulletClass,
			SocketLocation,
			SocketRotation,
			SpawnParams
		);

		FRotator AimRotation = FRotator();
		if (OwnerChar)
		{
			AimRotation = OwnerChar->GetControlRotation();
		}

		if (BulletActor)
		{
			BulletActor->SetOwner(GetOwner());
			BulletActor->SetInstigator(OwnerChar);
		}

		if (!OwnerChar->IsLocallyControlled())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketLocation,
				SocketRotation);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetActorLocation());

			if (ACharacter* Char = Cast<ACharacter>(OwnerChar))
			{
				if (!Char->HasAuthority()) Char->GetMesh()->GetAnimInstance()->Montage_Play(AimMontage);
				Char->GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Shoot"));
			}
		}
	}

	Mutlicast_PlayCosmetic(SocketLocation, SocketRotation);
}

void AWeaponActor::Mutlicast_PlayCosmetic_Implementation(FVector SocketLocation, FRotator SocketRotation)
{
	APawn* OwnerChar = Cast<APawn>(GetOwner());

	if (OwnerChar)
	{
		if (!OwnerChar->IsLocallyControlled())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketLocation,
				SocketRotation);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetActorLocation());

			if (ACharacter* Char = Cast<ACharacter>(OwnerChar))
			{
				Char->GetMesh()->GetAnimInstance()->Montage_Play(AimMontage);
				Char->GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Shoot"));
			}
		}
	}
}

void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentFireTimer <= FireRate && bIsFiring)
	{
		currentFireTimer += DeltaTime;

		if (currentFireTimer >= FireRate && bIsAuto)
		{
			currentFireTimer = 0;
			bIsFiring = false;
			if(bFirePressed) StartFire();
		}
		else if (currentFireTimer >= FireRate)
		{
			currentFireTimer = 0;
			bIsFiring = false;
		}
	}
	/*else if (currentFireTimer >= FireRate)
	{
		bIsFiring = false;
		currentFireTimer = 0;
	}*/
}
