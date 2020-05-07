// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TracerRangeWeapon.h"
#include "ART/ART.h"
#include "Player/BaseCharacter.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "AI/BaseAI.h"
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>
#include <Particles/ParticleSystemComponent.h>
#include "Net/UnrealNetwork.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "Ability/GATA_LineTrace.h"

static int32 DebugWeaponFiring = 0;
FAutoConsoleVariableRef CVARDebugWeaponFiring(
	TEXT("COOP.DebugWeapon"),
	DebugWeaponFiring,
	TEXT("Draw Debug For Weapon"),
	ECVF_Cheat);

ATracerRangeWeapon::ATracerRangeWeapon(){
	TracerTargetName = "Target";	
}

ATracerRangeWeapon::~ATracerRangeWeapon()
{
	if (IsValid(LineTraceTargetActor) && GetWorld() && !GetWorld()->bIsTearingDown)
	{
		LineTraceTargetActor->Destroy();
	}
}

AGATA_LineTrace* ATracerRangeWeapon::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = GetWorld()->SpawnActor<AGATA_LineTrace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

void ATracerRangeWeapon::BeginPlay(){
	Super::BeginPlay();
}

void ATracerRangeWeapon::Fire(){
	Super::Fire();

	FHitResult Hit;

	if (!OwningCharacter) {
		UE_LOG(LogTemp, Warning, TEXT("OwnerNotFound"));
	}
	else {
		FVector ShotPosition = GetActorLocation(); ;
		FVector ShotDirection = OwningCharacter->GetActorRotation().Vector();
		FVector TraceEnd = ShotPosition + (ShotDirection * Range);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwningCharacter);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//Particle "Target: parameter
		FVector TraceEndPoint = TraceEnd;
		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, ShotPosition, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			//block hit, process
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				ActualDamage *= 4;
			}


			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, OwningCharacter->GetInstigatorController(), this, DamageType);

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

			TraceEndPoint = Hit.ImpactPoint;
		}

		ThreatTrace();

		if (DebugWeaponFiring > 0) {
			DrawDebugLine(GetWorld(), ShotPosition, TraceEndPoint, FColor::Red, false, 1, 0, 1);
		}

		PlayFireEffect(TraceEndPoint);

		if (HasAuthority()) {
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
			HitScanTrace.StructIncreasement++;
			OnRep_HitScanTrace();
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}

}

void ATracerRangeWeapon::OnRep_HitScanTrace(){
	//play cosmetic effect
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ATracerRangeWeapon::ThreatTrace(){
	TArray<FHitResult> HitResults;
	FVector MuzzleLocation = StaticMeshComponent->GetSocketLocation(MuzzleSocketName);
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ThreatValue);
	AActor* MyOwner = GetOwner();
	FVector ShotPosition = GetActorLocation(); ;
	FVector ShotDirection = MyOwner->GetActorRotation().Vector();
	FVector TraceEnd = ShotPosition + (ShotDirection * Range);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	TArray<AActor*> IngnoredActors;
	IngnoredActors.Add(this);

	if (GetWorld()->SweepMultiByChannel(HitResults,
		MuzzleLocation, TraceEnd, FQuat::FQuat(),
		ECC_PhysicsBody, CollisionSphere, QueryParams)) {
		/*if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
			MuzzleLocation, TraceEnd, ThreatValue,
			ETraceTypeQuery::TraceTypeQuery2, true,
			IngnoredActors, EDrawDebugTrace::ForDuration,
			HitResults, true, FLinearColor::Blue, FLinearColor::Green, 2.0f)){ */
		for (FHitResult Hit : HitResults) {
			if (ABaseAI* AIPawn = Cast<ABaseAI>(Hit.Actor)) {
				//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.Actor->GetName());
				AIPawn->Threaten();
			}
		}
	}
}

void ATracerRangeWeapon::PlayFireEffect(FVector TraceEndPoint){
	if (TracerEffect) {
		FVector MuzzleLocation = StaticMeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}

	}
	APawn* MyOwner = Cast<APawn>(OwningCharacter);
	if (MyOwner) {
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC) {
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void ATracerRangeWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint){
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType) {
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect) {
		FVector MuzzleLocation = StaticMeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ATracerRangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ATracerRangeWeapon, HitScanTrace, COND_SkipOwner);
}