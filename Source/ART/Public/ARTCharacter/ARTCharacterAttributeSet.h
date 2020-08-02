// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ARTCharacterAttributeSet.generated.h"

/**
 * 
 */

 // Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ART_API UARTCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UARTCharacterAttributeSet();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//AttackPower
	UPROPERTY(BlueprintReadOnly, Category = "AttackPower", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, AttackPower)

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, AttackPower, OldAttackPower); }

	//Armor
	UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Armor)

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Armor, OldArmor); }

	//shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Shield)

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Shield, OldShield); }

	//max shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxShield)

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxShield, OldMaxShield); }

	//shieldregen
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_ShieldRegen)
	FGameplayAttributeData ShieldRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, ShieldRegen)

	UFUNCTION()
	void OnRep_ShieldRegen(const FGameplayAttributeData& OldShieldRegen) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, ShieldRegen, OldShieldRegen); }

	//Health
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Health)

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Health, OldHealth); }

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxHealth, OldMaxHealth); }

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, HealthRegen)

	UFUNCTION()
	void OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, HealthRegen, OldHealthRegen); }


	//Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Stamina)

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Stamina, OldStamina); }

	// MaxStamina is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxStamina)

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxStamina, OldMaxStamina); }

	//Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegen)
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, StaminaRegen)

	UFUNCTION()
	void OnRep_StaminaRegen(const FGameplayAttributeData& OldStaminaRegen) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, StaminaRegen, OldStaminaRegen); }

	//MoveSpeed
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MoveSpeed)

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MoveSpeed, OldMoveSpeed); }

	//RotateRate
	UPROPERTY(BlueprintReadOnly, Category = "RotateRate", ReplicatedUsing = OnRep_RotateRate)
	FGameplayAttributeData RotateRate;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, RotateRate)

	UFUNCTION()
	void OnRep_RotateRate(const FGameplayAttributeData& OldRotateRate) { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, RotateRate, OldRotateRate); }

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Damage)

	// Healing is a meta attribute used by the HealingExecution to calculate final healing, which then turns into +Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Healing", meta = (HideFromLevelInfos))
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Healing)

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
		
};
