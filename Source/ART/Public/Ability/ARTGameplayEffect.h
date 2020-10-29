// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ARTGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class ART_API UARTGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:

	     
	UPROPERTY(EditDefaultsOnly, Category = "ARTCurve")
	class UARTCurve* Curves;

protected:


};
