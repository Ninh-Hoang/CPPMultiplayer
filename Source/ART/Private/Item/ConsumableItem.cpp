// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem.h"
#include "Item/InventoryComponent.h"
#include "Ability/ARTGameplayAbility.h"
#include <AbilitySystemComponent.h>
#include "ARTCharacter/ARTSurvivor.h"

#define LOCTEXT_NAMESPACE "FoodItem"

UConsumableItem::UConsumableItem(){
	AmountToHeal = 100;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UConsumableItem::Use(AARTSurvivor* Character){
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Pawn's ASC is null."), *FString(__FUNCTION__));
		return;
	}

	for (TSubclassOf<UARTGameplayAbility> AbilityClass : AbilityClasses)
	{
		if (!AbilityClass)
		{
			continue;
		}

		ASC->GiveAbilityAndActivateOnce(FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(AbilityClass.GetDefaultObject()->AbilityInputID), this));
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> EffectClass : EffectClasses)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(EffectClass, Character->GetCharacterLevel(), EffectContext);

		if (NewHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}
}

#undef LOCTEXT_NAMESPACE
