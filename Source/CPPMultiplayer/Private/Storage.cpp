// Fill out your copyright notice in the Description page of Project Settings.


#include "Storage.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AStorage::AStorage(){
	SetReplicates(true);
	SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AStorage::BeginPlay(){
	Super::BeginPlay();
	if (InventoryComponent) {
		//InventoryComponent->Capacity = 20;
	}

}

void AStorage::InitializeComponents(UInventoryComponent* InventoryComp){
	InventoryComponent = InventoryComp;
}



