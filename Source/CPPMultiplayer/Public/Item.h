// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */
class UStaticMesh;
class UTexture2D;
class UInventoryComponent;
class ABaseCharacter;
class AActor;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class CPPMULTIPLAYER_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
    UItem();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    FText UseActionText;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    UStaticMesh* PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AActor> SpawnActor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* Thumbnail;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    FText ItemDisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta= (MultiLine = true))
    FText ItemDescription;

    UPROPERTY()
    UInventoryComponent* OwningInventory;

    virtual void Use(ABaseCharacter* Character);

    UFUNCTION(BlueprintImplementableEvent)
    void OnUse(ABaseCharacter* Character);
};
