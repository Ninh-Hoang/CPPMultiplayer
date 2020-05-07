// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RangeWeapon.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Player/BaseCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTCharacterBase.h"

ARangeWeapon::ARangeWeapon(){
	bAutomatic = true;
	RateOfFire = 600.f;
	Range = 10000.f;
	FirstShotDelay = 0.0f;
	FiringRotationSpeed = 50.f;
	AimRotationSpeed = 180.f;
	AimMovementSpeed = 200.f;
	FiringMovementSpeed = 100.f;
	MuzzleSocketName = "MuzzleSocket";
	OwnerRotationSpeed = 360.f;
	OwnerMovementSpeed = 200.f;

	PrimaryClipAmmo = 0;
	MaxPrimaryClipAmmo = 0;
	SecondaryClipAmmo = 0;
	MaxSecondaryClipAmmo = 0;
	bInfiniteAmmo = false;

	PrimaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));
	SecondaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));

	FireMode = FGameplayTag::RequestGameplayTag(FName("Weapon.FireMode.None"));
}

void ARangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARangeWeapon, PrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARangeWeapon, MaxPrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARangeWeapon, SecondaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARangeWeapon, MaxSecondaryClipAmmo, COND_OwnerOnly);
}

void ARangeWeapon::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(ARangeWeapon, PrimaryClipAmmo, (AbilitySystemComponent && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
	DOREPLIFETIME_ACTIVE_OVERRIDE(ARangeWeapon, SecondaryClipAmmo, (AbilitySystemComponent && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
}

void ARangeWeapon::ResetWeapon()
{
	Super::ResetWeapon();
	FireMode = DefaultFireMode;
}

int32 ARangeWeapon::GetPrimaryClipAmmo() const
{
	return PrimaryClipAmmo;
}

int32 ARangeWeapon::GetMaxPrimaryClipAmmo() const
{
	return MaxPrimaryClipAmmo;
}

int32 ARangeWeapon::GetSecondaryClipAmmo() const
{
	return SecondaryClipAmmo;
}

int32 ARangeWeapon::GetMaxSecondaryClipAmmo() const
{
	return MaxSecondaryClipAmmo;
}

void ARangeWeapon::SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo)
{
	int32 OldPrimaryClipAmmo = PrimaryClipAmmo;
	PrimaryClipAmmo = NewPrimaryClipAmmo;
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void ARangeWeapon::SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo)
{
	int32 OldMaxPrimaryClipAmmo = MaxPrimaryClipAmmo;
	MaxPrimaryClipAmmo = NewMaxPrimaryClipAmmo;
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void ARangeWeapon::SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo)
{
	int32 OldSecondaryClipAmmo = SecondaryClipAmmo;
	SecondaryClipAmmo = NewSecondaryClipAmmo;
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void ARangeWeapon::SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo)
{
	int32 OldMaxSecondaryClipAmmo = MaxSecondaryClipAmmo;
	MaxSecondaryClipAmmo = NewMaxSecondaryClipAmmo;
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}

bool ARangeWeapon::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

void ARangeWeapon::BeginPlay(){
	Super::BeginPlay();
	TimeBetweenShot = 60 / RateOfFire;
}

void ARangeWeapon::StartMouseOne(){
	Super::StartMouseOne();
	OffsetCharacterSpeedWhileFiring();
	StartFire();
}

void ARangeWeapon::StopMouseOne(){
	Super::StopMouseOne();
	OffsetCharacterSpeedWhileFiring();
	StopFire();
}

void ARangeWeapon::Fire(){
	if (!HasAuthority()) {
		ServerFire();
	}
	PlayMuzzleEffect();
}

void ARangeWeapon::PlayMuzzleEffect(){
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, StaticMeshComponent, MuzzleSocketName);
	}
}

void ARangeWeapon::StartFire(){
	float FirstDelay = FMath::Max<float>(LastFireTime + TimeBetweenShot - GetWorld()->TimeSeconds, 0);

	if (FirstShotDelay > 0) {
		FirstDelay = FMath::Max<float>(FirstDelay, FirstShotDelay);
	}

	if (bAutomatic) {
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShot, this, &ARangeWeapon::Fire, TimeBetweenShot, true, FirstDelay);
	}
	else {
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShot, this, &ARangeWeapon::Fire, TimeBetweenShot, false, FirstDelay);
	}

}

void ARangeWeapon::StopFire(){
	/*GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShot);

	if (UCharacterMovementComponent* OwnerMovementComp = OwningCharacter->FindComponentByClass<UCharacterMovementComponent>()) {
		if (OwningCharacter->bIsAiming) {
			OwnerMovementComp->RotationRate.Yaw = AimRotationSpeed;
			OwnerMovementComp->MaxWalkSpeed = AimMovementSpeed;
		}
		else {
			OwnerMovementComp->RotationRate.Yaw = OwnerRotationSpeed;
			OwnerMovementComp->MaxWalkSpeed = OwnerMovementSpeed;
		}
	}*/
}

void ARangeWeapon::StartMouseTwo(){
	Super::StartMouseTwo();
	OffSetCharacterSpeedWhileAiming();
}

void ARangeWeapon::StopMouseTwo(){
	Super::StopMouseTwo();
	OffSetCharacterSpeedWhileAiming();
}

void ARangeWeapon::OffsetCharacterSpeedWhileFiring() {
	/*if (UCharacterMovementComponent* OwnerMovementComp = OwningCharacter->FindComponentByClass<UCharacterMovementComponent>()) {
		float NewMovementSpeed;
		float NewRotationSpeed;
		if (OwningCharacter->bIsAttacking) {
			NewRotationSpeed = FiringRotationSpeed;
			NewMovementSpeed = FiringMovementSpeed;
		}
		else {
			if (OwningCharacter->bIsAiming) {
				NewRotationSpeed = AimRotationSpeed;
				NewMovementSpeed = AimMovementSpeed;
			}
			else {
				NewRotationSpeed = OwnerRotationSpeed;
				NewMovementSpeed = OwnerMovementSpeed;
			}
		}
		if (!HasAuthority()) {
			ServerChangeCharacterMovement(NewMovementSpeed, NewRotationSpeed);
		}
		OwnerMovementComp->RotationRate.Yaw = NewRotationSpeed;
		OwnerMovementComp->MaxWalkSpeed = NewMovementSpeed;
	}*/
}

void ARangeWeapon::OffSetCharacterSpeedWhileAiming(){
	/*if (OwningCharacter->bIsAttacking) {
		return;
	}
	if (UCharacterMovementComponent* OwnerMovementComp = OwningCharacter->FindComponentByClass<UCharacterMovementComponent>()) {
		if (!HasAuthority()) {
			ServerOffsetCharacterSpeedWhileAiming();
		}
		if (OwningCharacter->bIsAiming) {
			OwnerMovementComp->RotationRate.Yaw = AimRotationSpeed;
			OwnerMovementComp->MaxWalkSpeed = AimMovementSpeed;
		}
		else {
			OwnerMovementComp->RotationRate.Yaw = OwnerRotationSpeed;
			OwnerMovementComp->MaxWalkSpeed = OwnerMovementSpeed;
		}
	}*/
}

void ARangeWeapon::ServerFire_Implementation(){
	Fire();
}

bool ARangeWeapon::ServerFire_Validate(){
	return true;
}

void ARangeWeapon::ServerChangeCharacterMovement_Implementation(float MovementSpeed, float RotationRate){
	if(UCharacterMovementComponent* OwnerMovementComp = OwningCharacter->FindComponentByClass<UCharacterMovementComponent>()) {
		OwnerMovementComp->RotationRate.Yaw = RotationRate;
		OwnerMovementComp->MaxWalkSpeed = MovementSpeed;
	}
}

bool ARangeWeapon::ServerChangeCharacterMovement_Validate(float MovementSpeed, float RotationRate){
	return true;
}

void ARangeWeapon::OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo)
{
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void ARangeWeapon::OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo)
{
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void ARangeWeapon::OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo)
{
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void ARangeWeapon::OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo)
{
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}

