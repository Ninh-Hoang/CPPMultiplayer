// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ARTGameMode.h"
#include "AI/ARTAIManager.h"
#include "TimerManager.h"
#include "Framework/SGameState.h"

AARTGameMode::AARTGameMode()
{
	AIManager = CreateDefaultSubobject<UARTAIManager>(TEXT("AIMananger"));
}

void AARTGameMode::BeginPlay()
{
	Super::BeginPlay();
}

UARTAIManager* AARTGameMode::GetAIManager(const UObject* WorldContextObject)
{
	if(GEngine) {
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
		AARTGameMode * GameMode = World->GetAuthGameMode<AARTGameMode>();
		if(GameMode) return GameMode->AIManager;
	}
	return nullptr;
}
