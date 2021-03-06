// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "ARTAIConductor.generated.h"

class AARTCharacterAI;

UCLASS()
class ART_API UARTAIConductor : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UARTAIConductor();

	void Activate(bool bNewAutoActivate) override;

	UPROPERTY(BlueprintReadWrite)
	TMap<AActor*, FVector> ActorLocationMap;

protected:
	TArray<AARTCharacterAI*> AIList;

	TArray<FVector> MoveLocations;

public:	
	UFUNCTION(BlueprintPure, Category="AIManager")
	TArray<AARTCharacterAI*> GetAIList() const;

	UFUNCTION(BlueprintPure, Category="AIManager")
    TArray<FVector> GetAIMoveToLocation() const;
	
	UFUNCTION(BlueprintCallable, Category="AIManager")
    void AddAIToList(AARTCharacterAI* AI);

	UFUNCTION(BlueprintCallable, Category="AIManager")
    void RemoveAIFromList(AARTCharacterAI* AI);

	UFUNCTION(BlueprintCallable, Category="AIManager")
    void AddLocationToList(FVector Location);
};

