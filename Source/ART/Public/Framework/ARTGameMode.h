// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameMode.h"
#include "ARTGameMode.generated.h"

/**
 * 
 */
enum class EWaveState : uint8;

class UARTAIManager;

UCLASS()
class ART_API AARTGameMode : public AGameMode
{
	GENERATED_BODY()

	AARTGameMode();
	
	UPROPERTY()
    UARTAIManager* AIManager;
	
	void BeginPlay() override;
	
public:
	//Returns pointer to the AIDirector in the world
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "AIManager"), Category = "Level|LevelManager")
    static class UARTAIManager* GetAIManager(const UObject* WorldContextObject);
};
 