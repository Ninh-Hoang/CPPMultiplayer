// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableItem.h"
#include "InventoryComponent.h"

#define LOCTEXT_NAMESPACE "FoodItem"

UConsumableItem::UConsumableItem(){
	AmountToHeal = 100;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UConsumableItem::Use(ABaseCharacter* Character){
	if (Character) {
		if (OwningInventory) {
			OwningInventory->RemoveItem(this);
		}
	}
}

#undef LOCTEXT_NAMESPACE
