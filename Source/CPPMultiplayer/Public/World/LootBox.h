// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootBox.generated.h"

class USHealthComponent;
class UItem;

UCLASS()
class CPPMULTIPLAYER_API ALootBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootBox();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(BlueprintReadOnly, Category = "Health Component")
	USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Loot Box")
	TSubclassOf<AActor> Item;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "LootBox")
	void InitializeComponent(USHealthComponent* HealthComp);

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	void SpawnItem();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnItem();


};
