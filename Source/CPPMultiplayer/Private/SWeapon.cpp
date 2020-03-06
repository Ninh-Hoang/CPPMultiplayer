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

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay(){
	Super::BeginPlay();

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	
}

// Called every frame

void ASWeapon::Initialize(UMeshComponent* MeshComponentToSet){
	MeshComponent = MeshComponentToSet;
}
	
void ASWeapon::Fire()
{
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

		if (GetWorld()->LineTraceSingleByChannel(Hit, ShotPosition, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			//block hit, process

			AActor* HitActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(HitActor, 20, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			UParticleSystem* SelectedEffect = nullptr;
			if (SurfaceType == SurfaceType1 || SurfaceType == SurfaceType2) {
				UE_LOG(LogTemp, Warning, TEXT("FleshHit"));
			}
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
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TraceEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), ShotPosition, TraceEndPoint, FColor::Red, false, 1, 0, 1);
		}

		PlayerFireEffect(TraceEndPoint);
	}
}

void ASWeapon::Debug(){
	UE_LOG(LogTemp, Warning, TEXT("Working"));
}

void ASWeapon::PlayerFireEffect(FVector TraceEndPoint){
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

