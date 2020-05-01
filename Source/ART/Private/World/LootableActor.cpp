// Fill out your copyright notice in the Description page of Project Settings.


#include "World/LootableActor.h"
#include "Components/StaticMeshComponent.h"
#include "World/ItemSpawn.h"
#include "Item/Item.h"
#include "Item/InventoryComponent.h"
#include "Player/InteractionComponent.h"
#include "Engine/DataTable.h"
#include "Player/BaseCharacter.h"

#define LOCTEXT_NAMESPACE "LootableActor"
// Sets default values
ALootableActor::ALootableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//set up mesh
	LootContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>("LootContainerMesh");
	SetRootComponent(LootContainerMesh); 

	//setup loot interaction
	LootInteraction = CreateDefaultSubobject<UInteractionComponent>("LootInteraction");
	LootInteraction->InteractableActionText = LOCTEXT("LootActorText","Loot");
	LootInteraction->InteractableNameText = LOCTEXT("LootActorText", "Chest");
	LootInteraction->SetupAttachment(GetRootComponent());

	//setup inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	Inventory->SetCapacity(20);
	Inventory->SetWeightCapacity(80.0f);

	LootRolls = FIntPoint(2, 8);

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ALootableActor::BeginPlay(){
	Super::BeginPlay(); 

	LootInteraction->OnInteract.AddDynamic(this, &ALootableActor::OnInteract);

	if (HasAuthority() && LootTable) {
		TArray<FLootTableRow*> SpawnItems;
		LootTable->GetAllRows("", SpawnItems); 

		int32 Rolls = FMath::RandRange(LootRolls.GetMin(), LootRolls.GetMax());

		for (int32 i = 0; i < Rolls; i++) {
			const FLootTableRow* LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];

			ensure(LootRow);

			float ProbabilityRoll = FMath::FRandRange(0.f, 1.f);

			while (ProbabilityRoll > LootRow->Probability) {
				LootRow = SpawnItems[FMath::RandRange(0, SpawnItems.Num() - 1)];
				ProbabilityRoll = FMath::FRandRange(0.f, 1.f);
			}

			if (LootRow && LootRow->Items.Num()) {
				for (auto& ItemClass : LootRow->Items) {
					if (ItemClass) {
						const int32 Quantity = Cast<UItem>(ItemClass->GetDefaultObject())->GetQuantity();
						Inventory->TryAddItemFromClass(ItemClass, Quantity);
					}
				 }
			}
		}
	}
}

void ALootableActor::OnInteract(ABaseCharacter* Character){
	if (Character) {
		Character->SetLootSource(Inventory);
	}
}

#undef LOCTEXT_NAMESPACE
