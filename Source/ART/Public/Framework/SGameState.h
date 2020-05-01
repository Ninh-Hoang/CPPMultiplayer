// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWaveState : uint8 {
	WaitingToStart,
	WaveInProgress,

	//no longer spawn bot
	WaitingToComplete,
	WaveComplete,
	GameOver
};

UCLASS()
class ART_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent)
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:
	void SetWaveState(EWaveState NewState);
};
