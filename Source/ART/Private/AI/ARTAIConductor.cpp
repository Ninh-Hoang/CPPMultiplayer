// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ARTAIConductor.h"
#include "ARTCharacter/AI/ARTCharacterAI.h"
#include "Kismet/GameplayStatics.h"

UARTAIConductor::UARTAIConductor()
{
	SetAutoActivate(true);
}

void UARTAIConductor::Activate(bool bNewAutoActivate)
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

TArray<AARTCharacterAI*> UARTAIConductor::GetAIList() const
{
	return AIList;
}

TArray<FVector> UARTAIConductor::GetAIMoveToLocation()const
{
	return MoveLocations;
}

void UARTAIConductor::AddAIToList(AARTCharacterAI* AI)
{
	if(AI && !AI->IsPendingKillPending())
	{
		AIList.Add(AI);
	}
}

void UARTAIConductor::RemoveAIFromList(AARTCharacterAI* AI)
{
	AIList.RemoveSingle(AI);
	ActorLocationMap.Remove(AI);
}

void UARTAIConductor::AddLocationToList(FVector InLocation)
{
	MoveLocations.Add(InLocation);
}
