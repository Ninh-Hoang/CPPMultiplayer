// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Engine/ActorChannel.h"
#include "Item/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Player/InteractionComponent.h"
#include "Player/BaseCharacter.h"
#include "Item/InventoryComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APickup::APickup(){
	SetReplicates(true); 

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetRootComponent(PickupMesh);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("PickupInteractionComponent");
	InteractionComponent->InteractionTime = 0.5;
	InteractionComponent->InteractionDistance = 200.;
	InteractionComponent->InteractableNameText = FText::FromString("Pickup");
	InteractionComponent->InteractableActionText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &APickup::OnTakePickup);
	InteractionComponent->SetupAttachment(PickupMesh);
}

void APickup::InitializePickup(const TSubclassOf<UItem> ItemClass, const int32 Quantity){
	if (Role == ROLE_Authority && ItemClass && Quantity > 0) {
		Item = NewObject<UItem>(this, ItemClass);
		Item->SetQuantity(Quantity);
		OnRep_Item();
		Item->MarkDirtyForReplication();
	}
}

void APickup::OnRep_Item() {
	if (Item) {
		PickupMesh->SetStaticMesh(Item->PickupMesh);

		InteractionComponent->InteractableNameText = Item->ItemDisplayName;

		Item->OnItemModified.AddDynamic(this, &APickup::OnItemModified);
	}

	InteractionComponent->RefreshWidget();
}

void APickup::OnTakePickup(ABaseCharacter* Taker){
	if (!Taker) {
		UE_LOG(LogTemp, Warning, TEXT("Pickup was taken but player not valid."));
	}

	if (HasAuthority() && !IsPendingKill() && Item) {
		if (UInventoryComponent* PlayerInventory = Taker->InventoryComponent) {
			const FItemAddResult AddResult = PlayerInventory->TryAddItem(Item);
			if (AddResult.ActualAmountGiven < Item->GetQuantity()) {
				Item->SetQuantity(Item->GetQuantity() - AddResult.ActualAmountGiven);
			}
			else if (AddResult.ActualAmountGiven >= Item->GetQuantity()) {
				Destroy();
			}
		}
	}
}



void APickup::OnItemModified(){
	if (InteractionComponent) {
		InteractionComponent->RefreshWidget(); 
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay(){
	Super::BeginPlay();
	
	if (Role == ROLE_Authority && ItemTemplate && bNetStartup) {
		InitializePickup(ItemTemplate->GetClass(), ItemTemplate->GetQuantity());
	}

	if (!bNetStartup) {
		AlignWithGround();
	}
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, Item);
}

bool APickup::ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlags){
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Item){
		bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
	}

	return bWroteSomething;
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent){
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(APickup, ItemTemplate)) {
		if (ItemTemplate) {
			PickupMesh->SetStaticMesh(ItemTemplate->PickupMesh);
		}
	}
}
#endif
