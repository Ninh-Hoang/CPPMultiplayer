// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ConsumableItem.h"
#include "Item/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "FoodItem"

UConsumableItem::UConsumableItem(){
	AmountToHeal = 100;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UConsumableItem::Use(ABaseCharacter* Character){
	UE_LOG(LogTemp, Warning, TEXT("Consume item."))
	/*if (Character) {
		if (OwningInventory) {
			OwningInventory->RemoveItem(this);
		}
	}*/
}

#undef LOCTEXT_NAMESPACE
