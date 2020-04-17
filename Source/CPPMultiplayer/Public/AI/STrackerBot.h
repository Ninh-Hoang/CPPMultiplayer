// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;

UCLASS()
class CPPMULTIPLAYER_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//components
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USHealthComponent* HealthComp;

	UFUNCTION()
		void HandleTakeDamage(USHealthComponent* HealthComponent,
			float Health, float HealthDelta,
			const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	//next point in navigation
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TrackerBot", meta=(ClampMin = 0.f))
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TrackerBot", meta = (ClampMin = 0.f))
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamageFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float StartExplodingRange;

	//dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(ReplicatedUsing=OnRep_Explode, BlueprintReadOnly, Category = "TrackerBot")
	bool bExploded;

	bool bStartSelfDestruct;

	FTimerHandle TimerHandel_SelfDamage;

	UFUNCTION()
	void DamageSelf();

	void PlayEffect();

	UFUNCTION()
	void OnRep_Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
