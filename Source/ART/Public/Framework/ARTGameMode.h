// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameMode.h"
#include "ARTGameMode.generated.h"

/**
 * 
 */
enum class EWaveState : uint8;

class UARTAIConductor;

UCLASS()
class ART_API AARTGameMode : public AGameMode
{
	GENERATED_BODY()

	AARTGameMode();
	
	void BeginPlay() override;
	
public:

};
 