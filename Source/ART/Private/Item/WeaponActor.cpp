// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/MeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ART/ART.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "AI/BaseAI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapon"),
	DebugWeaponDrawing,
	TEXT("Draw Debug For Weapon"),
	ECVF_Cheat);

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	BaseDamage = 20;

	RateOfFire = 600;

	SetReplicates(true);

	ThreatValue = 70;

	FireRotationSpeed = 50;

	FirstShotDelay = 0.0f;

	Range = 10000;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay(){
	Super::BeginPlay();

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	TimeBetweenShot = 60 / RateOfFire;
}

// Called every frame

void AWeaponActor::Initialize(UMeshComponent* MeshComponentToSet){
	MeshComponent = MeshComponentToSet;
}
	
void AWeaponActor::Fire()
{	
	//use ServerFire() for clients
	if (!HasAuthority()) {
		ServerFire();
	}

	//trace to cross hair
	FHitResult Hit;
	AActor* MyOwner = GetOwner();
	if (!MyOwner) {
		UE_LOG(LogTemp, Warning, TEXT("OwnerNotFound"));
	}
	else {
		FVector ShotPosition = GetActorLocation(); ;
		FVector ShotDirection = MyOwner->GetActorRotation().Vector();
		FVector TraceEnd = ShotPosition + (ShotDirection * Range);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
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
			if(SurfaceType == SURFACE_FLESHVULNERABLE){
				ActualDamage *= 4;
			}

			
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

			TraceEndPoint = Hit.ImpactPoint;
		}

		ThreatTrace();

		if (DebugWeaponDrawing > 0) {
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

void AWeaponActor::OnRep_HitScanTrace(){
	//play cosmetic effect
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void AWeaponActor::StartFire(){
	if (UCharacterMovementComponent* OwnerMovementComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		OwnerRotationSpeed = OwnerMovementComp->RotationRate.Yaw;
		OwnerMovementComp->RotationRate = FRotator(0, FireRotationSpeed, 0);
	}

	float FirstDelay = FMath::Max<float>(LastFireTime + TimeBetweenShot - GetWorld()->TimeSeconds, 0);

	if (FirstShotDelay > 0) {
		FirstDelay = FMath::Max<float>(FirstDelay, FirstShotDelay);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShot, this, &AWeaponActor::Fire, TimeBetweenShot, true, FirstDelay);
}

void AWeaponActor::StopFire(){
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShot);

	if (UCharacterMovementComponent* OwnerMovementComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		OwnerMovementComp->RotationRate = FRotator(0, OwnerRotationSpeed, 0);
	}
}

void AWeaponActor::Debug(){
	UE_LOG(LogTemp, Warning, TEXT("Working"));
}

void AWeaponActor::ThreatTrace(){
	TArray<FHitResult> HitResults;
	FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
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

void AWeaponActor::ServerFire_Implementation(){
	Fire();
}

bool AWeaponActor::ServerFire_Validate(){
	return true;
}

void AWeaponActor::PlayFireEffect(FVector TraceEndPoint){
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}

	if (TracerEffect) {
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}

	}
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner) {
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC) {
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void AWeaponActor::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint){
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
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeaponActor, HitScanTrace, COND_SkipOwner);
}


