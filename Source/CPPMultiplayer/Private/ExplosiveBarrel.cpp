// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bDied = false;
	BarrelDamage = 100;
	BarrelExplosionRadius = 400;
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay(){
	Super::BeginPlay();
	SetBarrelMaterial();
	if (HealthComponent) {
		HealthComponent->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
	}
}

void AExplosiveBarrel::SetBarrelMaterial(){
	if (!BarrelMesh) { return; }
	if (!bDied) {		
		BarrelMesh->SetMaterial(0, BarrelTexture);
	}
	else {
		BarrelMesh->SetMaterial(0, BarrelTextureExploded);
	}
}

void AExplosiveBarrel::PlayExplosionEffect(){
	if (ExplosionEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
}

void AExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComp, float Health, 
	float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser){
	if (Health <= 0 && bDied == false) {
		bDied = true;
		PlayExplosionEffect();
		SetBarrelMaterial();
		FVector Origin;
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), BarrelDamage, Origin, BarrelExplosionRadius,);
	}
}

// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

void AExplosiveBarrel::InitializeComponents(USHealthComponent* HealthComp, UStaticMeshComponent* BarrelMeshToSet){
	HealthComponent = HealthComp;
	BarrelMesh = BarrelMeshToSet;
}




