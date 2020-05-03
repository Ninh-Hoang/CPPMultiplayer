// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Player/BaseCharacter.h"

AWeapon::AWeapon(){
	WeaponType = EWeaponType::WT_Range;
	WeaponReadySlot = EWeaponReadySlot::WRS_RightHand;

	BaseDamage = 20;
	ThreatValue = 0;
}

void AWeapon::EquipWeapon(){
	FText SocketText = UEnum::GetDisplayValueAsText(WeaponReadySlot);
	FName SocketName = FName(*SocketText.ToString());

	AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AWeapon::UnEquipWeapon(){
	FText SocketText = UEnum::GetDisplayValueAsText(EquipmentSlot);
	FName SocketName = FName(*SocketText.ToString());
	AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AWeapon::StartMouseOne(){
}

void AWeapon::StopMouseOne(){
}

void AWeapon::StartMouseTwo(){
}

void AWeapon::StopMouseTwo(){
}

void AWeapon::ThreatTrace(){
}

