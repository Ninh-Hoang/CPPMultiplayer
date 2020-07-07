// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ConsumableItem.generated.h"

/**
 * 
 */

class AARTSurvivor;

UCLASS()
class ART_API UConsumableItem : public UItem
{
	GENERATED_BODY()

public:
	UConsumableItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	float AmountToHeal;

protected:
	virtual void Use(AARTSurvivor* Character) override;
};
