// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class USHealthComponent;
class UStaticMeshComponent;
class UMaterial;

UCLASS()
class CPPMULTIPLAYER_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Health Component")
	bool bDied;

	UPROPERTY(BlueprintReadOnly, Category = "Health Component")
	USHealthComponent* HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Barrel")
	UStaticMeshComponent* BarrelMesh;

	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetBarrelMaterial();

	void PlayExplosionEffect();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

public:	

	UPROPERTY(BlueprintReadWrite, Category = "Barrel")
	float BarrelDamage;

	UPROPERTY(BlueprintReadWrite, Category = "Barrel")
	float BarrelExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Texture")
	UMaterial* BarrelTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Texture")
	UMaterial* BarrelTextureExploded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* ExplosionEffect;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeComponents(USHealthComponent* HealthComp, UStaticMeshComponent* BarrelMeshToSet);

};
