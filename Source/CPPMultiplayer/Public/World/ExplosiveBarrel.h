// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class USHealthComponent;
class UStaticMeshComponent;
class UMaterial;
class URadialForceComponent;

UCLASS()
class CPPMULTIPLAYER_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:

	UPROPERTY(ReplicatedUsing=OnRep_Explode, BlueprintReadOnly, Category = "Health Component")
	bool bDied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float BarrelDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float ExplosionDamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float ForceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float ForceStrenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float ExplosionImpluse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barrel")
	float ExplosionDelay;

	FTimerHandle ExplosionTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Health Component")
	USHealthComponent* HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Barrel")
	UStaticMeshComponent* BarrelMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> BarrelDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Texture")
	UMaterial* BarrelTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Texture")
	UMaterial* BarrelTextureExploded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	UParticleSystem* ExplosionEffect;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlayBarrelEffect();

	UFUNCTION()
	void ExplodeBarrel();

	UFUNCTION()
	void OnRep_Explode();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeComponents(USHealthComponent* HealthComp, 
		UStaticMeshComponent* BarrelMeshToSet, 
		URadialForceComponent* RadialForceComp);

	

	

};
