// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CPPMULTIPLAYER_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNewWave();
};
