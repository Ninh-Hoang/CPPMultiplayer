// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::Initialize(USkeletalMeshComponent* MeshComponentToSet){
	MeshComponent = MeshComponentToSet;
}

void ASWeapon::Fire()
{
	//trace to crosshair
	FHitResult Hit;
	AActor* MyOwner = GetOwner();
	if (!MyOwner) {
		//UE_LOG(LogTemp, Warning, TEXT("OwnerNotFound"));
	}
	else {
		FVector OwnerPosition = MyOwner->GetActorLocation(); ;
		FVector ShotDirection = MyOwner->GetActorRotation().Vector();
		FVector TraceEnd = OwnerPosition + (ShotDirection * 10000);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		if (GetWorld()->LineTraceSingleByChannel(Hit, OwnerPosition, TraceEnd, ECC_Visibility, QueryParams)) {
			//block hit, process

			AActor* HitActor = Hit.GetActor();
			if (HitActor) {
				UE_LOG(LogTemp, Warning, TEXT("%s"), *HitActor->GetName());
			}
			UGameplayStatics::ApplyPointDamage(HitActor, 20, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
		}
		DrawDebugLine(GetWorld(), OwnerPosition, TraceEnd, FColor::Cyan, false, 1, 0, 10);
		UE_LOG(LogTemp, Warning, TEXT("Firing"));
	}
}

void ASWeapon::Debug(){
	UE_LOG(LogTemp, Warning, TEXT("Working"));
}

