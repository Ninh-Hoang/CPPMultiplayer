// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/MeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

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

		//Particle "Target: parameter
		FVector TraceEndPoint = TraceEnd;
		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), ShotPosition, TraceEndPoint, FColor::Red, false, 1,0,1);
		}

		if (GetWorld()->LineTraceSingleByChannel(Hit, ShotPosition, TraceEnd, ECC_Visibility, QueryParams)) {
			//block hit, process

			AActor* HitActor = Hit.GetActor();
			if (HitActor) {
				UE_LOG(LogTemp, Warning, TEXT("%s"), *HitActor->GetName());
			}
			UGameplayStatics::ApplyPointDamage(HitActor, 20, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			if (ImpactEffect) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			TraceEndPoint = Hit.ImpactPoint;
		}
		//DrawDebugLine(GetWorld(), ShotPosition, TraceEnd, FColor::Cyan, false, 1, 0, 5);
		UE_LOG(LogTemp, Warning, TEXT("Firing"));

		if (!MeshComponent) {
			UE_LOG(LogTemp, Warning, TEXT("No Mesh"));
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
}

