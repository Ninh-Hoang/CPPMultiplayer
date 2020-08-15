// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "ARTAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class ART_API UARTAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	UARTAbilitySystemGlobals();

	UPROPERTY()
	FGameplayTag DeadTag;

	UPROPERTY()
	FGameplayTag KnockedDownTag;

	UPROPERTY()
	FGameplayTag InteractingTag;

	UPROPERTY()
	FGameplayTag InteractingRemovalTag;

	static UARTAbilitySystemGlobals& ARTGet()
	{
		return dynamic_cast<UARTAbilitySystemGlobals&>(Get());
	}

	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

	virtual void InitGlobalTags() override;
};
