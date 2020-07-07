// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTSurvivor.h"
#include "ARTCharacter/ARTPlayerState.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "ARTCharacter/ARTCharacterBase.h"
#include <Camera/CameraComponent.h>

void AARTSurvivor::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}
