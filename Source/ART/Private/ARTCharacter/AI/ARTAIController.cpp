// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/AI/ARTAIController.h"
#include <ARTCharacter/ARTCharacterBase.h>

AARTAIController::AARTAIController()
{
}

class UAbilitySystemComponent* AARTAIController::GetAbilitySystemComponent() const
{
	if (GetPawn() && Cast<AARTCharacterBase>(GetPawn()))
	{
		return Cast<AARTCharacterBase>(GetPawn())->GetAbilitySystemComponent();
	}
	return nullptr;
}
