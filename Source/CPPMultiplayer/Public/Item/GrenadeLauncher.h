// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CPPMULTIPLAYER_API AGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()
	

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGrenadeFire();
};

