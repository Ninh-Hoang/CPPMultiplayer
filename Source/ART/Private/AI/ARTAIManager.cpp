// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ARTAIManager.h"
#include "ARTCharacter/AI/ARTCharacterAI.h"
#include "Kismet/GameplayStatics.h"

UARTAIManager::UARTAIManager()
{
	SetAutoActivate(true);
}

void UARTAIManager::Activate(bool bNewAutoActivate)
{
	Super::Activate();
	
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AARTCharacterAI::StaticClass(),AllActors);
	
	TArray<AARTCharacterAI*> AIActors;

	for(auto& Actor : AllActors)
	{
		AIActors.Add(Cast<AARTCharacterAI>(Actor));
	}
	
	AIList.Append(AIActors);

	UE_LOG(LogTemp, Warning, TEXT("Number of AI at Start: %i"), AIList.Num());
}

TArray<AARTCharacterAI*> UARTAIManager::GetAIList() const
{
	return AIList;
}

TArray<FVector> UARTAIManager::GetAIMoveToLocation()const
{
	return MoveLocations;
}

void UARTAIManager::AddAIToList(AARTCharacterAI* AI)
{
	if(AI && AI->IsPendingKillPending())
	{
		AIList.Add(AI);
	}
}

void UARTAIManager::AddLocationToList(FVector InLocation)
{
	MoveLocations.Add(InLocation);
}
