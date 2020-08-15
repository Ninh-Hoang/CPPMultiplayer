// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTPlayerState.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"
#include <ARTCharacter/ARTSurvivor.h>
#include <Ability/ARTAbilitySystemGlobals.h>

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
	DeadTag = UARTAbilitySystemGlobals::ARTGet().DeadTag;
	KnockedDownTag = UARTAbilitySystemGlobals::ARTGet().KnockedDownTag;
}

class UAbilitySystemComponent* AARTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UARTCharacterAttributeSet* AARTPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AARTPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 AARTPlayerState::GetCharacterLevel() const
{
	return 1;
}

float AARTPlayerState::GetAttackPower() const
{
	return AttributeSetBase->GetAttackPower();
}

float AARTPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AARTPlayerState::GetShield() const
{
	return AttributeSetBase->GetShield();
}

float AARTPlayerState::GetMaxShield() const
{
	return AttributeSetBase->GetMaxShield();
}

float AARTPlayerState::GetShieldRegen() const
{
	return AttributeSetBase->GetShieldRegen();
}

float AARTPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AARTPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AARTPlayerState::GetHealthRegen() const
{
	return AttributeSetBase->GetHealthRegen();
}

float AARTPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AARTPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

float AARTPlayerState::GetStaminaRegen() const
{
	return AttributeSetBase->GetStaminaRegen();
}

float AARTPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

float AARTPlayerState::GetRotateRate() const
{
	return AttributeSetBase->GetRotateRate();
}

void AARTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AARTPlayerState::HealthChanged);
	}
}

void AARTPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	if (!Survivor) {
		Survivor = Cast<AARTSurvivor>(GetPawn());
	}

	// Check for and handle knockdown and death
	if (IsValid(Survivor) && !IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		//Survivor->Die();
	}
}

	