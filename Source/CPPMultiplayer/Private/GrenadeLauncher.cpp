// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeLauncher.h"
#include "Net/UnrealNetwork.h"


void AGrenadeLauncher::Fire() {
	ServerGrenadeFire();
}

void AGrenadeLauncher::ServerGrenadeFire_Implementation(){
	FHitResult Hit;
	AActor* MyOwner = GetOwner();
	if (!MyOwner || !ProjectileClass) {
		UE_LOG(LogTemp, Warning, TEXT("Null"));
	}
	else {
		FVector ShotPosition = GetActorLocation(); ;
		FRotator ShotRotation = MyOwner->GetActorRotation();
		FVector ShotDirection = ShotRotation.Vector();
		FVector TraceEnd = ShotPosition + (ShotDirection * 10000);

		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComponent->GetSocketRotation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, ShotRotation, SpawnParams);
	}
}

bool AGrenadeLauncher::ServerGrenadeFire_Validate() {
	return true;
}





