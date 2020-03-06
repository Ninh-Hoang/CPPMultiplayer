// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class ASWeapon;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	FTimerHandle AimTimerHandler;

	UFUNCTION()
	void LookAtCursor();

	bool IsAiming;

	UCharacterMovementComponent* CharacterMovementComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	ASWeapon* CurrentWeapon;

	void Fire();

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttackSocketName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent = nullptr;;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* AzimuthComponent = nullptr;;

	void BeginCrouch();
	void EndCrouch();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeComponents(UCameraComponent* CameraToSet, 
			USpringArmComponent* SpringArmToSet);

	virtual FVector GetPawnViewLocation() const override;
	void Aim();
};
