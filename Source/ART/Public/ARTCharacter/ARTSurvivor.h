// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "ARTSurvivor.generated.h"

/**
 * 
 */
UCLASS()
class ART_API AARTSurvivor : public AARTCharacterBase
{
	GENERATED_BODY()

public:

	FGameplayTag CurrentWeaponTag;

	// Cache tags
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

protected:


};
