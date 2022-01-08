// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARTCharacter/ARTSurvivor.h"
#include "WDPlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class WIZARDDOMAIN_API AWDPlayerPawn : public AARTSurvivor
{
	GENERATED_BODY()

public:
	AWDPlayerPawn(const class FObjectInitializer& ObjectInitializer);
	
};
