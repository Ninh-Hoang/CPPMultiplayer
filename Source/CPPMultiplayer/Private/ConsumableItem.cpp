// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableItem.h"

UConsumableItem::UConsumableItem(){
	AmountToHeal = 100;
	ItemDisplayName = FText::FromString("Consumable Item");
	UseActionText = FText::FromString("Consume");
}

void UConsumableItem::Use(ABaseCharacter* Character){
	UE_LOG(LogTemp, Warning, TEXT("Consume Item"));
}
