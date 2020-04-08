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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0.0))
	float WeightCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0))
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UItem*> Items;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags);

public:
	UInventoryComponent();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnDefaultItem();
		
	bool AddItem(UItem* Item);

	bool RemoveItem(UItem* Item);

private: 
	UPROPERTY()
	int32 ReplicatedItemsKey;	 

	UFUNCTION()
	void OnRep_Items();
};
