// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ARTAbilitySystemGlobals.h"
#include <Ability/ARTGameplayEffectTypes.h>

UARTAbilitySystemGlobals::UARTAbilitySystemGlobals()
{
}

FGameplayEffectContext* UARTAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FARTGameplayEffectContext();
}

void UARTAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	KnockedDownTag = FGameplayTag::RequestGameplayTag("State.KnockedDown");
	InteractingTag = FGameplayTag::RequestGameplayTag("State.Interacting");
	InteractingRemovalTag = FGameplayTag::RequestGameplayTag("State.InteractingRemoval");
}
