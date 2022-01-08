// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ARTCharacter/ARTAttributeSetBase.h"
#include "WDCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class WIZARDDOMAIN_API UWDCharacterAttributeSet : public UARTAttributeSetBase
{
	GENERATED_BODY()

public:
	UWDCharacterAttributeSet();

	//AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const override;

	virtual void FinalDamageDealing(float LocalDamage, const FHitResult* Hit);

	//AttackPower
	UPROPERTY(BlueprintReadOnly, Category = "AttackPower", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, AttackPower)

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, AttackPower, OldAttackPower);
	}

	//CritRate
	UPROPERTY(BlueprintReadOnly, Category = "Crit", ReplicatedUsing = OnRep_CritRate)
	FGameplayAttributeData CritRate;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, CritRate)

	UFUNCTION()
	void OnRep_CritRate(const FGameplayAttributeData& OldCritRate)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, CritRate, OldCritRate);
	}

	//CritMultiplier
	UPROPERTY(BlueprintReadOnly, Category = "Crit", ReplicatedUsing = OnRep_CritMultiplier)
	FGameplayAttributeData CritMultiplier;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, CritMultiplier)

	UFUNCTION()
	void OnRep_CritMultiplier(const FGameplayAttributeData& OldCritMultiplier)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, CritMultiplier, OldCritMultiplier);
	}

	//physical damage bonus
	UPROPERTY(BlueprintReadOnly, Category = "Phys", ReplicatedUsing = OnRep_PhysBonus)
	FGameplayAttributeData PhysBonus;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, PhysBonus)

	UFUNCTION()
	void OnRep_PhysBonus(const FGameplayAttributeData& OldPhysBonus)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, PhysBonus, OldPhysBonus);
	}

	//physical damage res
	UPROPERTY(BlueprintReadOnly, Category = "Phys", ReplicatedUsing = OnRep_PhysRes)
	FGameplayAttributeData PhysRes;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, PhysRes)

	UFUNCTION()
	void OnRep_PhysRes(const FGameplayAttributeData& OldPhysRes)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, PhysRes, OldPhysRes);
	}

	//Armor
	UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Armor)

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, Armor, OldArmor);
	}

	//shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Shield)

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, Shield, OldShield);
	}

	//max shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, MaxShield)

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, MaxShield, OldMaxShield);
	}

	//ShieldRegen
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_ShieldRegen)
	FGameplayAttributeData ShieldRegen;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, ShieldRegen)

	UFUNCTION()
	void OnRep_ShieldRegen(const FGameplayAttributeData& OldShieldRegen)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, ShieldRegen, OldShieldRegen);
	}

	//Health
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Health)

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, Health, OldHealth);
	}

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, MaxHealth, OldMaxHealth);
	}

	// HealthRegen is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, HealthRegen)

	UFUNCTION()
	void OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, HealthRegen, OldHealthRegen);
	}

	//Energy
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Energy)

	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldEnergy)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, Energy, OldEnergy);
	}

	// MaxEnergy is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, MaxEnergy)

	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, MaxEnergy, OldMaxEnergy);
	}

	// EnergyRegen is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_EnergyRegen)
	FGameplayAttributeData EnergyRegen;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, EnergyRegen)

	UFUNCTION()
	void OnRep_EnergyRegen(const FGameplayAttributeData& OldEnergyRegen)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, EnergyRegen, OldEnergyRegen);
	}

	//MoveSpeed
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, MoveSpeed)

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, MoveSpeed, OldMoveSpeed);
	}

	//RotateRate
	UPROPERTY(BlueprintReadOnly, Category = "RotateRate", ReplicatedUsing = OnRep_RotateRate)
	FGameplayAttributeData RotateRate;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, RotateRate)

	UFUNCTION()
	void OnRep_RotateRate(const FGameplayAttributeData& OldRotateRate)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWDCharacterAttributeSet, RotateRate, OldRotateRate);
	}

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Damage)

	// Healing is a meta attribute used by the HealingExecution to calculate final healing, which then turns into +Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Healing", meta = (HideFromLevelInfos))
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UWDCharacterAttributeSet, Healing)

protected:
};
