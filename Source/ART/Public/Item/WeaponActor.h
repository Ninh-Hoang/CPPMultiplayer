// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UDamageType;
class UParticleSystem;
class UMeshComponent;


//contain weapon of LineTrace
USTRUCT()
struct FHitScanTrace {
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	UPROPERTY()
	uint8 StructIncreasement;
};

UCLASS()
class ART_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponActor();

public:
	UFUNCTION(BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintCallable)
	void StopFire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

protected:

	float LastFireTime;

	FTimerHandle TimerHandle_TimeBetweenShot;

	//RPM
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	//derived from RateOfFire
	float TimeBetweenShot;

	UPROPERTY(EditDefaultsOnly)
	float ThreatValue;

	UPROPERTY(EditDefaultsOnly)
	float FirstShotDelay;

	UPROPERTY(EditDefaultsOnly)
	float FireRotationSpeed; 

	float OwnerRotationSpeed;

	UPROPERTY(EditDefaultsOnly)
	float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UMeshComponent* MeshComponent;
	
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	virtual void BeginPlay() override;
		
	void PlayFireEffect(FVector TraceEndPoint);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	virtual void Fire();
	
	UFUNCTION()
	void OnRep_HitScanTrace();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialize(UMeshComponent* MeshComponentToSet);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Debug();

	void ThreatTrace();
};