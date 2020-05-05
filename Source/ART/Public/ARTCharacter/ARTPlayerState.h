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
UCLASS()
class ART_API AARTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AARTPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UARTCharacterAttributeSet* GetAttributeSetBase() const;
protected:
	UPROPERTY()
	class UARTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UARTCharacterAttributeSet* AttributeSetBase;

	FGameplayTag DeadTag;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTPlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTPlayerState|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTPlayerState|Attributes")
	float GetMoveSpeed() const;

};
