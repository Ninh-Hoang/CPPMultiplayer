// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAI.generated.h"

UCLASS()
class CPPMULTIPLAYER_API ABaseAI : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttackSocketName;

	bool bIsAiming;

	FTimerHandle AimTimerHandler;

	AActor* TargetActor;

	UPROPERTY(BlueprintReadOnly)
	ASWeapon* CurrentWeapon;

public:
	// Sets default values for this character's properties
	ABaseAI();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USHealthComponent* HealthComponent;

	class UCharacterMovementComponent* CharacterMovementComponent;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartAim(AActor* AimActor);

	UFUNCTION()
	void AimAtLocation();

	UFUNCTION(BlueprintCallable)
	void StopAim();

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(TSubclassOf<ASWeapon> WeaponToChange);
	
	//crouch
	UFUNCTION(BlueprintCallable)
	void BeginCrouch();

	UFUNCTION(BlueprintCallable)
	void EndCrouch();
public:	
	UFUNCTION(BlueprintImplementableEvent)
	void Threaten();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
