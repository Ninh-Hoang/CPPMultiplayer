// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Item.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Capacity = 20;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay(){
	Super::BeginPlay();

	// ...
	if (GetOwnerRole() == ROLE_Authority) {
		ServerSpawnDefaultItem();
	}
}

void UInventoryComponent::ServerSpawnDefaultItem_Implementation(){
	for (UItem* Item : DefaultItems) {
		AddItem(Item);
	}
}

bool UInventoryComponent::ServerSpawnDefaultItem_Validate(){
	return true;
}

bool UInventoryComponent::AddItem(UItem* Item) {
	if (Items.Num() >= Capacity || !Item) {
		return false;
	}
	Item->OwningInventory = this;
	Items.Add(Item);
	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item){
	if (Item) {
		Item->OwningInventory = NULL;
		Items.RemoveSingle(Item);
		OnInventoryUpdated.Broadcast();
		return true;
	}
	return false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, DefaultItems);
	DOREPLIFETIME(UInventoryComponent, Items);
}

// Called every frame

