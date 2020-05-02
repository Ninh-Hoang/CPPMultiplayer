// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Equipment.h"
#include "Weapon.generated.h"

/**
 * 
 */
UENUM()
enum class EWeaponType : uint8 {
	WT_Range UMETA(DisplayName = "MeleeWeapon"),
	WT_Melee UMETA(DisplayName = "RangeWeapon"),
};

UENUM()
enum class EWeaponReadySlot: uint8 {
	WRS_RightHand UMETA(DisplayName = "WeaponSocket"),
};


UCLASS(Abstract, Blueprintable, BlueprintType)
class ART_API AWeapon : public AEquipment
{
	GENERATED_BODY()
public:
	AWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	EWeaponReadySlot WeaponReadySlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly)
	float ThreatValue;

	virtual void ThreatTrace();

public: 
	virtual void EquipWeapon();
	virtual void UnEquipWeapon();
};
