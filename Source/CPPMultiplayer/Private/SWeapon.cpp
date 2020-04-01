// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/MeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CPPMultiplayer/CPPMultiplayer.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapon"),
	DebugWeaponDrawing,
	TEXT("Draw Debug For Weapon"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	BaseDamage = 20;

	RateOfFire = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay(){
	Super::BeginPlay();

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	TimeBetweenShot = 60 / RateOfFire;
}

// Called every frame

void ASWeapon::Initialize(UMeshComponent* MeshComponentToSet){
	MeshComponent = MeshComponentToSet;
}
	
void ASWeapon::Fire()
{	
	//use ServerFire() for clients
	if (Role < ROLE_Authority) {
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
		FVector TraceEnd = ShotPosition + (ShotDirection * 10000);
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

		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), ShotPosition, TraceEndPoint, FColor::Red, false, 1, 0, 1);
		}

		PlayFireEffect(TraceEndPoint);

		if (Role == ROLE_Authority) {
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
			HitScanTrace.StructIncreasement++;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::OnRep_HitScanTrace(){
	//play cosmetic effect
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::StartFire(){
	float FirstDelay = FMath::Max<float>(LastFireTime + TimeBetweenShot - GetWorld()->TimeSeconds, 0);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShot, this, &ASWeapon::Fire, TimeBetweenShot, true, FirstDelay);
}

void ASWeapon::StopFire(){
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShot);
}

void ASWeapon::Debug(){
	UE_LOG(LogTemp, Warning, TEXT("Working"));
}

void ASWeapon::ServerFire_Implementation(){
	Fire();
}

bool ASWeapon::ServerFire_Validate(){
	return true;
}

void ASWeapon::PlayFireEffect(FVector TraceEndPoint){
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

void ASWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint){
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

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}


