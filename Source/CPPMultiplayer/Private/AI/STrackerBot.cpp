// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STrackerBot.h"
#include <Components/StaticMeshComponent.h>
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Combat/SHealthComponent.h"
#include "Player/BaseCharacter.h"
#include <Components/SphereComponent.h>
#include "Net/UnrealNetwork.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	SetRootComponent(MeshComp);
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
	bUseVelocityChange = false;

	

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	ExplosionDamage = 100;
	ExplosionRadius = 200;

	SelfDamageFrequency = 5;
	StartExplodingRange = 200; 

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(StartExplodingRange);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);
	bExploded = false;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay(){
	Super::BeginPlay();

	if (HasAuthority()) {
		NextPathPoint = GetNextPathPoint();
	}
}

void ASTrackerBot::Tick(float DeltaTime)
{
	if (HasAuthority() && !bExploded) {
		Super::Tick(DeltaTime);

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget) {
			NextPathPoint = GetNextPathPoint();
		}
		else {
			//moving toward next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}
	}
}

FVector ASTrackerBot::GetNextPathPoint(){
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath =  UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath && NavPath->PathPoints.Num() > 1) {
		//return next point in path
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* HealthComponent,
	float Health, float HealthDelta,
	const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser) {

	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst) {
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f) {
		SelfDestruct();
	}
}

void ASTrackerBot::SelfDestruct(){
	if (HasAuthority()) {
		if (bExploded) {
			return;
		}

		bExploded = true;

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true);

		Destroy();
	}
}

void ASTrackerBot::DamageSelf(){
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::OnRep_Explode(){
	UE_LOG(LogTemp, Warning, TEXT("Explode"));
	PlayEffect();
}

void ASTrackerBot::PlayEffect() {
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor){
	if (!bStartSelfDestruct && !bExploded) {
		//if overlapped with player
		if (ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor)) {
			if (Role == ROLE_Authority) {
				GetWorldTimerManager().SetTimer(TimerHandel_SelfDamage, this, &ASTrackerBot::DamageSelf, 1 / SelfDamageFrequency, true, 0.0f);
			}
			bStartSelfDestruct = true;
		}
	}
}

void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTrackerBot, bExploded);
}
