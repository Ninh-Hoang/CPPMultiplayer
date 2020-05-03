// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
//range weapon type
UENUM()
enum class ERangeWeaponType : uint8 {
	WT_Tracer UMETA(DisplayName = "TracerRangeWeapon"),
	WT_Projectile UMETA(DisplayName = "ProjectileRangeWeapon"),
};

UCLASS(Abstract, Blueprintable, BlueprintType)
class ART_API ARangeWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	ARangeWeapon();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	ERangeWeaponType RangeWeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	bool bAutomatic;
	//RPM
	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float FirstShotDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float FiringRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float AimRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float FiringMovementSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Range Weapon")
	float AimMovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	FTimerHandle TimerHandle_TimeBetweenShot;

	//derived from RateOfFire
	float TimeBetweenShot;

	float LastFireTime;

	float OwnerRotationSpeed;
	float OwnerMovementSpeed;

	virtual void BeginPlay() override;

	//firing 
	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();

	//muzzle effect
	virtual void PlayMuzzleEffect();

	//chain of functions from player input
	virtual void StartMouseOne() override;
	virtual void StopMouseOne() override;

	virtual void StartFire();
	virtual void StopFire();

	virtual void StartMouseTwo() override;
	virtual void StopMouseTwo() override;

	virtual void OffsetCharacterSpeedWhileFiring();
	virtual void OffSetCharacterSpeedWhileAiming();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeCharacterMovement(float MovementSpeed, float RotationRate);
};
