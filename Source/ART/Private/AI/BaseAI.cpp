// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAI.h"
#include "Combat/SHealthComponent.h"
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/Actor.h>
#include <DrawDebugHelpers.h>
#include "ART/ART.h"
#include "Components/CapsuleComponent.h"
#include "Item/WeaponActor.h"

// Sets default values
ABaseAI::ABaseAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));
	CharacterMovementComponent = FindComponentByClass<UCharacterMovementComponent>();

	bIsAiming = false;

	//item, weapon
	WeaponAttackSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ABaseAI::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon(StarterWeaponClass);
}

void ABaseAI::StartAim(AActor* AimActor) {
	if (TargetActor != AimActor) {
		TargetActor = AimActor;
	}

	if (!bIsAiming){
		if (CharacterMovementComponent) {
			CharacterMovementComponent->bOrientRotationToMovement = false;
			CharacterMovementComponent->bUseControllerDesiredRotation = true;
		}
		bIsAiming = true;;

		GetWorld()->GetTimerManager().SetTimer(AimTimerHandler,
			this,
			&ABaseAI::AimAtLocation,
			GetWorld()->GetDeltaSeconds(),
			true);
	}
} 


void ABaseAI::AimAtLocation(){
	if (CurrentWeapon) {
		FVector WeaponLocation = CurrentWeapon->GetActorLocation();
		FRotator AimAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponLocation, TargetActor->GetActorLocation());
		GetController()->SetControlRotation(AimAtRotation);
	}
	else {
		FVector ActorLocation = GetActorLocation();
		FRotator AimAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetActor->GetActorLocation());
		GetController()->SetControlRotation(AimAtRotation);
	}
}

void ABaseAI::StopAim(){
	if (bIsAiming) {
		bIsAiming = false;

		if (CharacterMovementComponent) {
			CharacterMovementComponent->bOrientRotationToMovement = true;
			CharacterMovementComponent->bUseControllerDesiredRotation = false;
		}
		GetWorld()->GetTimerManager().ClearTimer(AimTimerHandler);

	}
}
// Called every frame
void ABaseAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseAI::EquipWeapon(TSubclassOf<AWeaponActor> WeaponToChange) {
	if (CurrentWeapon && StarterWeaponClass == WeaponToChange) {
		return;
	}
	else {
		StarterWeaponClass = WeaponToChange;
	}

	if (CurrentWeapon) {
		CurrentWeapon->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponToChange, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttackSocketName);
	}
}

void ABaseAI::BeginCrouch(){
	Crouch();
}

void ABaseAI::EndCrouch(){
	UnCrouch();
}

