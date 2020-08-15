// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TracerRangeWeapon.h"
#include "ART/ART.h"
#include "Player/BaseCharacter.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "AI/BaseAI.h"
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
	TArray<FHitResult> HitResults;
	FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ThreatValue);
	AActor* MyOwner = GetOwner();
	FVector ShotPosition = GetActorLocation(); ;
	FVector ShotDirection = MyOwner->GetActorRotation().Vector();
	FVector TraceEnd = ShotPosition + (ShotDirection * Range);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	TArray<AActor*> IngnoredActors;
	IngnoredActors.Add(this);

	if (GetWorld()->SweepMultiByChannel(HitResults,
		MuzzleLocation, TraceEnd, FQuat::FQuat(),
		ECC_PhysicsBody, CollisionSphere, QueryParams)) {
		/*if(UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
			MuzzleLocation, TraceEnd, ThreatValue,
			ETraceTypeQuery::TraceTypeQuery2, true,
			IngnoredActors, EDrawDebugTrace::ForDuration,
			HitResults, true, FLinearColor::Blue, FLinearColor::Green, 2.0f)){ */
		for (FHitResult Hit : HitResults) {
			if (ABaseAI* AIPawn = Cast<ABaseAI>(Hit.Actor)) {
				//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.Actor->GetName());
				AIPawn->Threaten();
			}
		}
	}
}