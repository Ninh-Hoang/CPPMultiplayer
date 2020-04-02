// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);
	bDied = false;
	BarrelDamage = 100;
	ExplosionDamageRadius = 300;
	ForceRadius = 300;
	ExplosionImpluse = 500;
	ForceStrenght = 500;
	ExplosionDelay = 1;
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay(){
	Super::BeginPlay();
	if (BarrelMesh && BarrelTexture) {
		BarrelMesh->SetMaterial(0, BarrelTexture);
	}
	if (BarrelMesh) {
		BarrelMesh->SetCollisionObjectType(ECC_PhysicsBody);
	}
	if (HealthComponent) {
		HealthComponent->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
	}
	if (RadialForceComponent) {
		RadialForceComponent->Radius = ForceRadius;
		RadialForceComponent->ImpulseStrength = ForceStrenght;
		RadialForceComponent->bImpulseVelChange = true;
		RadialForceComponent->bAutoActivate = true;
		RadialForceComponent->bIgnoreOwningActor = true;
	}
}

void AExplosiveBarrel::PlayBarrelEffect(){
	if (!BarrelMesh) { return; }
	if (!bDied && BarrelTexture && BarrelTextureExploded) {
		BarrelMesh->SetMaterial(0, BarrelTexture);
	}
	else {
		BarrelMesh->SetMaterial(0, BarrelTextureExploded);
	}

	if (ExplosionEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
}

void AExplosiveBarrel::ExplodeBarrel(){
	PlayBarrelEffect();

	if (ExplosionEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}

	FVector BoostVector = FVector::UpVector * ExplosionImpluse;
	BarrelMesh->AddImpulse(BoostVector, NAME_None, true);
	RadialForceComponent->FireImpulse();

	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, BarrelDamage, 
		GetActorLocation(), ExplosionDamageRadius, 
		BarrelDamageType, IgnoreActors, this, NULL, true);

	GetWorld()->GetTimerManager().ClearTimer(ExplosionTimer);
}

void AExplosiveBarrel::OnRep_Explode(){
	PlayBarrelEffect();
}

void AExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComp, float Health, 
	float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser){
	if (Health <= 0 && bDied == false) {
		bDied = true;
		OnRep_Explode();
		GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AExplosiveBarrel::ExplodeBarrel, GetWorld()->DeltaTimeSeconds, false, ExplosionDelay);
	}
}

void AExplosiveBarrel::InitializeComponents(USHealthComponent* HealthComp, UStaticMeshComponent* BarrelMeshToSet, URadialForceComponent* RadialForceComp){
	HealthComponent = HealthComp;
	BarrelMesh = BarrelMeshToSet;
	RadialForceComponent = RadialForceComp;
}

void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AExplosiveBarrel, bDied);
}




