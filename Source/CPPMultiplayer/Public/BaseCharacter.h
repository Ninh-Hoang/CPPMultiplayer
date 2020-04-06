// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class ASWeapon;
class USHealthComponent;
class UItem;
class UInventoryComponent;

UCLASS()
class CPPMULTIPLAYER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	void LookRight(float AxisValue);

	void StartFire();

	void StopFire();

	UFUNCTION()
	void LookAtCursor();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(Replicated)
	bool IsAiming;

	FTimerHandle AimTimerHandler;

	UCharacterMovementComponent* CharacterMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	//pawn die previously
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ASWeapon* CurrentWeapon;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttackSocketName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* AzimuthComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	void BeginCrouch();
	void EndCrouch();

	void Aim();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAim();

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeComponents(UCameraComponent* CameraToSet,
	USpringArmComponent* SpringArmToSet, USHealthComponent* HealthComp, UInventoryComponent* InventoryComp);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseItem(UItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(UItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void ChangeWeapon(TSubclassOf<ASWeapon> WeaponToChange);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeWeapon(TSubclassOf<ASWeapon> WeaponToChange);
	
	
};
