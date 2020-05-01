// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/SGameMode.h"
#include "TimerManager.h"
#include "Combat/SHealthComponent.h"
#include "Framework/SGameState.h"

ASGameMode::ASGameMode(){
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	BotCountMultiplier = 2;
	WaveCount = 0;
	TimeBetweenWave = 2;
	GameStateClass = ASGameState::StaticClass();
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

	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave(){
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	PrepareForNewWave();

	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNewWave(){
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, & ASGameMode::StartWave, TimeBetweenWave, false);

	SetWaveState(EWaveState::WaitingToStart);
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
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNewWave();
	}
}

void ASGameMode::CheckAnyPlayerAlive(){
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn()) {
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp)&& HealthComp->GetHealth() > 0.0f) {
				// a player is still alive
				return;
			}
		}
	}

	//no player alive
	GameOver();
}

void ASGameMode::GameOver(){
	EndWave();

	SetWaveState(EWaveState::GameOver);

	//finish up the match, present 'game over' to players.
	UE_LOG(LogTemp, Log, TEXT("Game Over"));
}

void ASGameMode::SetWaveState(EWaveState NewState){
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS)) {
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::StartPlay(){
	Super::StartPlay();

	PrepareForNewWave();
}

void ASGameMode::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	CheckWaveState();

	CheckAnyPlayerAlive();
}
