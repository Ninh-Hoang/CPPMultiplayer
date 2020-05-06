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

	//shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Shield)

	UFUNCTION()
	void OnRep_Shield() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Shield); }

	//max shield
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxShield)

	UFUNCTION()
	void OnRep_MaxShield() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxShield); }

	//shieldregen
	UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_ShieldRegen)
	FGameplayAttributeData ShieldRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, ShieldRegen)

	UFUNCTION()
	void OnRep_ShieldRegen() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, ShieldRegen); }

	//Health
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Health)

	UFUNCTION()
	void OnRep_Health() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Health); }

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_MaxHealth() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxHealth); }

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, HealthRegen)

	UFUNCTION()
	void OnRep_HealthRegen() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, HealthRegen); }


	//Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Stamina)

	UFUNCTION()
	void OnRep_Stamina() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, Stamina); }

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MaxStamina)

	UFUNCTION()
	void OnRep_MaxStamina() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MaxStamina); }

	//Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegen)
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, StaminaRegen)

	UFUNCTION()
	void OnRep_StaminaRegen() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, StaminaRegen); }

	//MoveSpeed
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, MoveSpeed)

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UARTCharacterAttributeSet, Damage)

	UFUNCTION()
	void OnRep_MoveSpeed() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MoveSpeed); }

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	
};
