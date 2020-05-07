// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

class AARTCharacterBase;

UENUM()
enum class EEquipmentSlot: uint8 {
	ES_WeaponRest UMETA(DisplayName = "WeaponRestSocket"),
	ES_RightHand UMETA(DisplayName = "WeaponSocket"),
	ES_Pelvis UMETA(DisplayName = "PelvisSocket"),
	ES_Shield UMETA(DisplayName = "ShieldSocket"),
};

UCLASS(Abstract, Blueprintable, BlueprintType)
class ART_API AEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipment();

	UPROPERTY(VisibleDefaultsOnly, Category = "Equipment")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Equipment")
	class AARTCharacterBase* OwningCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	EEquipmentSlot EquipmentSlot;

public:	

	virtual void Equip(class AARTCharacterBase* CharacterToEquip);
	virtual void UnEquip();

};
