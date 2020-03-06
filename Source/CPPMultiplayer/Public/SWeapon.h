// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class UDamageType;
class UParticleSystem;
class UMeshComponent;

UCLASS()
class CPPMULTIPLAYER_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UMeshComponent* MeshComponent;

	virtual void Fire();

	FTimerHandle TimerHandle_TimeBetweenShot;

	float LastFireTime;

	//RPM
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	//derived from RateOfFire
	float TimeBetweenShot;

public:

	void StartFire();

	void StopFire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

protected:

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialize(UMeshComponent* MeshComponentToSet);



	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Debug();

	void PlayerFireEffect(FVector TraceEndPoint);

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

};