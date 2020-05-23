// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Equipment.h"
#include "Player/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include <Components/SkeletalMeshComponent.h>

// Sets default values
AEquipment::AEquipment()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Azimuth"));
	SetRootComponent(SkeletalMeshComponent);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquipmentSlot = EEquipmentSlot::ES_Shield;
}

// Called when the game starts or when spawned
void AEquipment::BeginPlay()
{
	Super::BeginPlay();
}

void AEquipment::Equip(AARTCharacterBase* CharacterToEquip){
	//stupid conversion from enum to FName, holy shit
	/*const TEnumAsByte<EEquipmentSlot> Enum = EquipmentSlot;
	FString Enum = UEnum::GetValueAsString(Enum.GetValue());
	FName Socket = FName(*Enum);*/

	FText SocketText = UEnum::GetDisplayValueAsText(EquipmentSlot);
	FName SocketName = FName(*SocketText.ToString());

	SetOwner(CharacterToEquip);
	AttachToComponent(CharacterToEquip->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

	OwningCharacter = CharacterToEquip;
}

void AEquipment::UnEquip(){

}

USkeletalMeshComponent* AEquipment::GetEquipmentMesh()
{
	return SkeletalMeshComponent;
}

//replication
void AEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AEquipment, OwningCharacter, COND_OwnerOnly);
}

