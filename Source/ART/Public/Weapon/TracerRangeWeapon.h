// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/RangeWeapon.h"
#include "TracerRangeWeapon.generated.h"

/**
 * 
 */
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
class ART_API ATracerRangeWeapon : public ARangeWeapon
{
	GENERATED_BODY()
public:
	ATracerRangeWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	virtual void BeginPlay() override;

	virtual void Fire() override;

	UFUNCTION()
	void OnRep_HitScanTrace();

	virtual void ThreatTrace() override;

	void PlayFireEffect(FVector TraceEndPoint);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);
};
