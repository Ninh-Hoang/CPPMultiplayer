// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "ARTSurvivor.generated.h"

/**
 * 
 */

class AWeapon;

USTRUCT()
struct ART_API FSurvivorEquipment
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<AWeapon*> Weapons;

	// Consumable items

	// Passive items like armor

	// Door keys

	// Etc
};

UCLASS()
class ART_API AARTSurvivor : public AARTCharacterBase
{
	GENERATED_BODY()

public:

	AARTSurvivor(const class FObjectInitializer& ObjectInitializer);

	FGameplayTag CurrentWeaponTag;

	// Cache tags
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|Inventory")
	AWeapon* GetCurrentWeapon() const;

protected:
	//EQUIPMENT LIST
	UPROPERTY(ReplicatedUsing = OnRep_Equipment)
	FSurvivorEquipment Equipment;

	UFUNCTION()
	void OnRep_Equipment();

	//WEAPON STUFFS

	//current weapon
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	AWeapon* CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	UFUNCTION(Server, WithValidation, Reliable)
	void ServerSyncCurrentWeapon();

	UFUNCTION(Client, WithValidation, Reliable)
	void ClientSyncCurrentWeapon(AWeapon* InWeapon);

	//equip new weapon
	UFUNCTION(BlueprintCallable, Category = "GASShooter|Inventory")
	void EquipWeapon(AWeapon* NewWeapon);

	UFUNCTION(Server, WithValidation, Reliable)
	void ServerEquipWeapon(AWeapon* NewWeapon);

	// Unequips the specified weapon. Used when OnRep_CurrentWeapon fires.
	void UnEquipWeapon(AWeapon* WeaponToUnEquip);

	void UnEquipCurrentWeapon();

	//add weapon to equipment
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AWeapon* AddWeaponToEquipment(TSubclassOf<AWeapon> WeaponClass);

	bool DoesWeaponExistInInventory(AWeapon* InWeapon);

	//ITEM USING / INVENTORY
public:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class APickup> PickupClass;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseItem(UItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(UItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem(UItem* Item, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropItem(UItem* Item, int32 Quantity);
};