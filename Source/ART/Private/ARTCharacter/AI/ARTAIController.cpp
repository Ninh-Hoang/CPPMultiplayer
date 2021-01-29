// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/AI/ARTAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "ARTCharacter/ARTCharacterBase.h"

AARTAIController::AARTAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
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
