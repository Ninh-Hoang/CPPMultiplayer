// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Item.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Capacity = 20;
	SetIsReplicated(true);
	// ...
}

FItemAddResult UInventoryComponent::TryAddItem(UItem* Item){
	return TryAddItem_Internal(Item); 
}

FItemAddResult UInventoryComponent::TryAddItemFromClass(TSubclassOf<UItem> ItemClass, const int32 Quantity){
	UItem* Item = NewObject<UItem>(GetOwner(), ItemClass);
	Item->SetQuantity(Quantity);
	return TryAddItem_Internal(Item);
}

bool UInventoryComponent::RemoveItem(UItem* Item){
	if (GetOwner() && GetOwner()->HasAuthority() {

	}
}

void UInventoryComponent::ClientRefreshInventory_Implementation(){

}

UItem* UInventoryComponent::AddItem(UItem* Item){
	if (GetOwner() && GetOwner()->HasAuthority()) {
		UItem* NewItem = NewObject<UItem>(GetOwner(), Item->GetClass());
		NewItem->SetQuantity(Item->GetQuantity());
		NewItem->OwningInventory = this;
		NewItem->AddedToInventory(this); 
		Items.Add(Item);
		NewItem->MarkDirtyForReplication();

		return NewItem;
	}

	return nullptr;
}

FItemAddResult UInventoryComponent::TryAddItem_Internal(UItem* Item){
	AddItem(Item);
	return FItemAddResult::AddedAll(Item->Quantity);
}

void UInventoryComponent::OnRep_Items() {
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);
}

bool UInventoryComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags){
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags); 

	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey)) {
		for (UItem* Item : Items) {
			if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->RepKey)) {
				bWroteSomething	|= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}
	}
	return bWroteSomething;
}

