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
	ASGameMode();

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	//amount of bot spawn in a single wave
	int32 AmountOfBotToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 BotCountMultiplier;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWave;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapse();

	void StartWave();

	void EndWave();

	void PrepareForNewWave();

	void CheckWaveState();

public:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
