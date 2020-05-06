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

	UFUNCTION()
	void OnRep_MoveSpeed() { GAMEPLAYATTRIBUTE_REPNOTIFY(UARTCharacterAttributeSet, MoveSpeed); }

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	
};
