// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "MyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

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
	if (!bCanShoot) return;

	bFirePressed = true;
	if (!bIsFiring) StartFire();
}

void AWeaponActor::StartFire()
{
	if (!bCanShoot)
	{
		AMyPlayerCharacter* Char = Cast<AMyPlayerCharacter>(GetOwner());
		if (Char) Char->OnAutoWeaponNeedsReload();
		EndFire();
		return;
	}

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
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLoc,
		AimPoint,
		ECC_Visibility, QueryParams))
	{
		AimPoint = Hit.ImpactPoint;
		bool bHeadShoot = Hit.BoneName == FName("head");
		
		if (ACharacter* InHitPlayer = Cast<ACharacter>(Hit.GetActor()))
		{
			OnWeaponFireHit(InHitPlayer, nullptr, Hit);
		}

		OnWeaponHitTarget.Broadcast(Hit.GetActor(), bHeadShoot);
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

		// Detact magazine count
		if (CurrentMaganizeSize > 0)
		{
			--CurrentMaganizeSize;
			bCanShoot = CurrentMaganizeSize > 0;
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

	OnWeaponFired();

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


void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	InteractMesh->SetSimulatePhysics(true);

	if (HasAuthority())
	{
		CurrentMaganizeSize = MaxMaganizeSize;
		bCanShoot = true;
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

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponActor, CurrentBulletsCount);
	DOREPLIFETIME(AWeaponActor, CurrentMaganizeSize);
	DOREPLIFETIME(AWeaponActor, bCanShoot);
}

void AWeaponActor::AddBullets(int32 BulletNum)
{
	if (!HasAuthority()) return;

	int32 targetBulletNum = CurrentBulletsCount + BulletNum;

	if (targetBulletNum < MaxBulletsCount)
	{
		CurrentBulletsCount = targetBulletNum;
	}
	else
		CurrentBulletsCount = MaxBulletsCount;
}

void AWeaponActor::ReloadWeapon()
{
	if (!HasAuthority()) return;

	if (CurrentBulletsCount <= 0) return;

	int32 MagazineSizeToAdd = MaxMaganizeSize - CurrentMaganizeSize;

	if ((CurrentBulletsCount - MagazineSizeToAdd) > 0)
	{
		CurrentMaganizeSize += MagazineSizeToAdd;
		CurrentBulletsCount -= MagazineSizeToAdd;
	}
	else
	{
		CurrentMaganizeSize += CurrentBulletsCount;
		CurrentBulletsCount = 0;
	}

	bCanShoot = true;
}


void AWeaponActor::OnRep_CanInteract()
{
	if (bCanInteract)
		InteractMesh->SetSimulatePhysics(true);
	else
	{
		InteractMesh->SetSimulatePhysics(false);

		if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
		{
			AttachToComponent(MyOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}
