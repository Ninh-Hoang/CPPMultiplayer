// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPPMULTIPLAYER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	

public:	
	UPROPERTY(Replicated, EditDefaultsOnly, Instanced)
	TArray<UItem*> DefaultItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Inventory")
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UItem*> Items;

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnDefaultItem();

	bool AddItem(UItem* Item);

	bool RemoveItem(UItem* Item);
};
