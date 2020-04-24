// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SGameMode.h"
#include "TimerManager.h"
#include "Combat/SHealthComponent.h"

ASGameMode::ASGameMode(){
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	BotCountMultiplier = 2;
	WaveCount = 0;
	TimeBetweenWave = 2;
}

void ASGameMode::SpawnBotTimerElapse(){
	SpawnNewBot();

	AmountOfBotToSpawn--;

	if (AmountOfBotToSpawn <= 0) {
		EndWave();
	}
}

void ASGameMode::StartWave(){
	WaveCount++;
	AmountOfBotToSpawn = BotCountMultiplier * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnNewBot, 1.0f, true, 0);
}

void ASGameMode::EndWave(){
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	PrepareForNewWave();
}

void ASGameMode::PrepareForNewWave(){
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, & ASGameMode::StartWave, TimeBetweenWave, false);
}

void ASGameMode::CheckWaveState(){
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (AmountOfBotToSpawn > 0 || bIsPreparingForWave) {
		return;
	}

	bool bIsAnyBotAlive;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) {
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) {
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f) {
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive) {
		PrepareForNewWave();
	}
}

void ASGameMode::StartPlay(){
	Super::StartPlay();

	PrepareForNewWave();
}

void ASGameMode::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}
