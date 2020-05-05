// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTPlayerState.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"

AARTPlayerState::AARTPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UARTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UARTCharacterAttributeSet>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

class UAbilitySystemComponent* AARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UARTCharacterAttributeSet* AARTPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

float AARTPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();

}

float AARTPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AARTPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}
