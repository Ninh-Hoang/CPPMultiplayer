// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/MMC/ARTEnergyCost_MMC.h"
#include <ARTCharacter/ARTCharacterAttributeSet.h>


UARTEnergyCost_MMC::UARTEnergyCost_MMC()
{
	EnergyDef.AttributeToCapture = UARTCharacterAttributeSet::GetEnergyAttribute();
	EnergyDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	EnergyDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(EnergyDef);
}

float UARTEnergyCost_MMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return 0.f;
}
