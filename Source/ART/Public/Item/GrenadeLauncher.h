// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponActor.h"
#include "GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class ART_API AGrenadeLauncher : public AWeaponActor
{
	GENERATED_BODY()
	

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGrenadeFire();
};


