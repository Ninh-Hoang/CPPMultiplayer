// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ARTAIController.generated.h"

/**
 * 
 */
UCLASS()
class ART_API AARTAIController : public AAIController
{
	GENERATED_BODY()

	AARTAIController();

	class UAbilitySystemComponent* GetAbilitySystemComponent() const;
};
