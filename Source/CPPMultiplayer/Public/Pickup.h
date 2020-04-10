// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class UItem;
class UActorChannel;
class UStaticMeshComponent;
class UInteractionComponent;

UCLASS()
class CPPMULTIPLAYER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UItem* ItemTemplate;

	void InitializePickup(const	TSubclassOf<UItem> ItemClass, const int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent)
	void AlignWithGround();

	

protected:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = OnRep_Item)
	UItem* Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	UInteractionComponent* InteractionComponent;

	UFUNCTION()
	void OnTakePickup(ABaseCharacter* Taker);

	UFUNCTION()
	void OnRep_Item();

	//refresh UI/Notification
	UFUNCTION()
	void OnItemModified();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	bool ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlag) override;

#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent);
#endif
};
