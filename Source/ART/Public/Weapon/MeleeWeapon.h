// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class ART_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeleeWeapon();
	~AMeleeWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY()
	class AGATA_MeleeWeaponTrace* MeeleeTargetActor;

	// Getter for MeeleeTargetActor. Spawns it if it doesn't exist yet.
	UFUNCTION(BlueprintCallable, Category = "GASShooter|Targeting")
	class AGATA_MeleeWeaponTrace* GetMeeleeTraceTargetActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponStartLength = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponTail = 120;

	UFUNCTION(BlueprintCallable, Category ="Melee Weapon")
	void StartTrace();

	UFUNCTION(BlueprintCallable, Category = "Melee Weapon")
	void EndTrace();

protected:
	TArray<FVector> PrevVecs;
	TArray<FVector> CurrVecs;

	FTimerHandle TraceTimerHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<AActor*> MyTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ExposeOnSpawn = 1))
	bool bEnableDrawTraceLine;

	void CheckCollision();

	bool TryExcludeActor(AActor* HitActor);

	void DrawTraceLine(FVector prevVec_, FVector currVec_, bool bDrawTraceLine);

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyEventBackToGA(AARTCharacterBase* Target, const FHitResult& InpHitResult);
};
