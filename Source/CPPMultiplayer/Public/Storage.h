// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Storage.generated.h"

class UInventoryComponent;

UCLASS()
class CPPMULTIPLAYER_API AStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	UInventoryComponent* Inventory;

public:	
	UFUNCTION(BlueprintCallable)
	void InitializeComponents(UInventoryComponent* InventoryComp);
};
