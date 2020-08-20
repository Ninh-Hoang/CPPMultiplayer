// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TracerRangeWeapon.h"
#include "ART/ART.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>
#include <Particles/ParticleSystemComponent.h>
#include "Net/UnrealNetwork.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "Ability/TargetActor/GATA_LineTrace.h"

static int32 DebugWeaponFiring = 0;
FAutoConsoleVariableRef CVARDebugWeaponFiring(
	TEXT("COOP.DebugWeapon"),
	DebugWeaponFiring,
	TEXT("Draw Debug For Weapon"),
	ECVF_Cheat);

ATracerRangeWeapon::ATracerRangeWeapon(){
	TracerTargetName = "Target";	
}

ATracerRangeWeapon::~ATracerRangeWeapon()
{
	if (IsValid(LineTraceTargetActor) && GetWorld() && !GetWorld()->bIsTearingDown)
	{
		LineTraceTargetActor->Destroy();
	}
}

AGATA_LineTrace* ATracerRangeWeapon::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = GetWorld()->SpawnActor<AGATA_LineTrace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

void ATracerRangeWeapon::BeginPlay(){
	Super::BeginPlay();
}
void ATracerRangeWeapon::ThreatTrace(){
}