// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/AI/ARTCharacterAI.h"
#include <Ability/ARTAbilitySystemComponent.h>
#include <ARTCharacter/ARTCharacterAttributeSet.h>
#include <Components/CapsuleComponent.h>



AARTCharacterAI::AARTCharacterAI(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Create Ability System Component
	HardRefAbilitySystemComponent = CreateDefaultSubobject<UARTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HardRefAbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Set our parent's TWeakObjectPtr
	AbilitySystemComponent = HardRefAbilitySystemComponent;

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UARTCharacterAttributeSet>(TEXT("AttributeSetBase"));

	// Set our parent's TWeakObjectPtr
	AttributeSetBase = HardRefAttributeSetBase;

	//set collision
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AARTCharacterAI::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();

		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AARTCharacterAI::HealthChanged);

	}
}

void AARTCharacterAI::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// If the minion died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Die();
	}
}
