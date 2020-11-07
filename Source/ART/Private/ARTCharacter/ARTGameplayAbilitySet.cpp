// Fill out your copyright notice in the Description page of Project Settings.
#include "ARTCharacter/ARTGameplayAbilitySet.h"
#include <Ability/ARTGameplayEffect.h>
#include <Ability/ARTGameplayAbility.h>
#include <Ability/ARTAbilitySystemComponent.h>

void UARTGameplayAbilitySet::GiveAbilities(UARTAbilitySystemComponent* AbilitySystemComponent) const
{

	for (const FARTGameplayAbilityApplicationInfo& GameplayAbility : StartupGameplayAbilities)
	{
		if (GameplayAbility.GameplayAbilityClass)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(GameplayAbility.GameplayAbilityClass, 
					(int32)GameplayAbility.Level,
					static_cast<int32>(GameplayAbility.GameplayAbilityClass.GetDefaultObject()->AbilityInputID), 
					AbilitySystemComponent->GetAvatarActor()));
		}
	}
}

void UARTGameplayAbilitySet::AddStartupEffects(class UARTAbilitySystemComponent* AbilitySystemComponent) const
{
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(AbilitySystemComponent->GetAvatarActor());

	for (const FARTGameplayEffectApplicationInfo& GameplayEffect : StartupGameplayEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect.GameplayEffectClass, GameplayEffect.Level, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}
