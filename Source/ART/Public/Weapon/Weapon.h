// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Equipment.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystemInterface.h"
#include "ART/ART.h"
#include "Weapon.generated.h"

/**
 * 
 */
class UARTGameplayAbility;
class UAbilitySystemComponent;
class UARTAbilitySystemComponent;
class UAnimMontage;
class UAnimInstance;
class AARTCharacterBase;

UENUM()
enum class EWeaponType : uint8 {
	WT_Range UMETA(DisplayName = "MeleeWeapon"),
	WT_Melee UMETA(DisplayName = "RangeWeapon"),
};

UENUM()
enum class EWeaponReadySlot: uint8 {
	WRS_RightHandGun UMETA(DisplayName = "GunSocket"),
	WRS_RightHandMelee UMETA(DisplayName = "MeleeSocket"),
};


UCLASS(Abstract, Blueprintable, BlueprintType)
class ART_API AWeapon : public AEquipment, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AWeapon();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|GSWeapon")
		FGameplayTag WeaponTag;

protected:

	UPROPERTY()
	UARTAbilitySystemComponent* AbilitySystemComponent;

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
	
	UPROPERTY(EditAnywhere, Category = "GASShooter|GSWeapon")
	TArray<TSubclassOf<UARTGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "GASShooter|GSWeapon")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Animation")
	UAnimMontage* EquipMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Animation")
	TSubclassOf<UAnimInstance> AnimSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|GSWeapon")
	FText DefaultStatusText;
		
	// Cache tags
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponSecondaryInstantAbilityTag;
	FGameplayTag WeaponAlternateInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;

	virtual void BeginPlay() override;

public: 
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "GASShooter|GSWeapon")
	FText StatusText;

	void SetOwningCharacter(AARTCharacterBase* InOwningCharacter);

	virtual void AddAbilities();

	virtual void RemoveAbilities();

	virtual int32 GetAbilityLevel(EARTAbilityInputID AbilityID);

	// Resets things like fire mode to default
	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSWeapon")
	virtual void ResetWeapon();

	UFUNCTION(BlueprintCallable, Category = "GASShooter|Animation")
	UAnimMontage* GetEquipMontage() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSWeapon")
	FText GetDefaultStatusText() const;

	UFUNCTION(BlueprintCallable)
	virtual void EquipWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void UnEquipWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void StartMouseOne();

	UFUNCTION(BlueprintCallable)
	virtual void StopMouseOne();

	UFUNCTION(BlueprintCallable)
	virtual void StartMouseTwo();

	UFUNCTION(BlueprintCallable)
	virtual void StopMouseTwo();


};
