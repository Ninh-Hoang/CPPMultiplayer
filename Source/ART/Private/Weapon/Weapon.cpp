// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Player/BaseCharacter.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "Ability/ARTGameplayAbility.h"
#include "ARTCharacter/ARTCharacterBase.h"

AWeapon::AWeapon(){
	WeaponType = EWeaponType::WT_Range;
	WeaponReadySlot = EWeaponReadySlot::WRS_RightHandGun;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 100.0f;

	BaseDamage = 20;
	ThreatValue = 0;

	WeaponPrimaryInstantAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.Primary.Instant"));
	WeaponSecondaryInstantAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.Secondary.Instant"));
	WeaponAlternateInstantAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.Alternate.Instant"));
	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag(FName("Weapon.IsFiring"));

	StatusText = DefaultStatusText;
}

UAbilitySystemComponent* AWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWeapon::EquipWeapon() {
	if (OwningCharacter) {
		FText SocketText = UEnum::GetDisplayValueAsText(WeaponReadySlot);
		FName SocketName = FName(*SocketText.ToString());

		AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
		if (AnimSet) {
			OwningCharacter->GetMesh()->SetAnimInstanceClass(AnimSet);
		}
	}
}

void AWeapon::UnEquipWeapon(){
	if (OwningCharacter){
	FText SocketText = UEnum::GetDisplayValueAsText(EquipmentSlot);
		FName SocketName = FName(*SocketText.ToString());
		AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
}

void AWeapon::ThreatTrace(){
}

void AWeapon::BeginPlay()
{
	ResetWeapon();
	Super::BeginPlay();
}

void AWeapon::SetOwningCharacter(AARTCharacterBase* InOwningCharacter){
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter) {
		AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
	}
}

void AWeapon::AddAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UARTAbilitySystemComponent* ASC = Cast<UARTAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s Role: %s ASC is null"), *FString(__FUNCTION__), *GetName(), GET_ACTOR_ROLE_FSTRING(OwningCharacter));
		return;
	}

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UARTGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, GetAbilityLevel(Ability.GetDefaultObject()->AbilityID), static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
		UE_LOG(LogTemp, Warning, TEXT("Add Ability"));
	}
}

void AWeapon::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UARTAbilitySystemComponent* ASC = Cast<UARTAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	// Remove abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

int32 AWeapon::GetAbilityLevel(EARTAbilityInputID AbilityID)
{
	// All abilities for now are level 1
	return 1;
}

void AWeapon::ResetWeapon()
{
	StatusText = DefaultStatusText;
}

UAnimMontage* AWeapon::GetEquipMontage() const
{
	return EquipMontage;
}

FText AWeapon::GetDefaultStatusText() const
{
	return DefaultStatusText;
}

