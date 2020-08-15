// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "ARTPlayerState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FARTOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);


UCLASS()
class ART_API AARTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AARTPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UARTCharacterAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetAttackPower() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetShieldRegen() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetHealthRegen() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStaminaRegen() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMoveSpeed() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetRotateRate() const;

protected:
	UPROPERTY()
	class UARTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UARTCharacterAttributeSet* AttributeSetBase;

	FGameplayTag DeadTag;
	FGameplayTag KnockedDownTag;

	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;

	class AARTSurvivor* Survivor;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
