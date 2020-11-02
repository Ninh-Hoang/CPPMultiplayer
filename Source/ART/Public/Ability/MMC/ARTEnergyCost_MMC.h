// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ARTEnergyCost_MMC.generated.h"

/**
 * 
 */
UCLASS()
class ART_API UARTEnergyCost_MMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UARTEnergyCost_MMC();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition EnergyDef;
};
