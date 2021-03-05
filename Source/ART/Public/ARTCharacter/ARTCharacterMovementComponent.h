// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ARTCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ART_API UARTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FARTSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		///@brief Resets all saved variables.
		virtual void Clear() override;

		///@brief Store input commands in the compressed flags.
		virtual uint8 GetCompressedFlags() const override;

		///@brief This is used to check whether or not two moves can be combined into one.
		///Basically you just check to make sure that the saved variables are the same.
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		///@brief Sets up the move before sending it to the server. 
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
		                        class FNetworkPredictionData_Client_Character& ClientData) override;
		///@brief Sets variables on character movement component before making a predictive correction.
		virtual void PrepMoveFor(class ACharacter* Character) override;

		// Sprint
		uint8 SavedRequestToStartSprinting : 1;

		// Aim Down Sights
		uint8 SavedRequestToStartADS : 1;

		// Aim Down Sights
		uint8 SavedRequestToStartBlocking : 1;

		// Aim Down Sights
		uint8 SavedRequestToStartAttacking : 1;
	};

	class FARTNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FARTNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		///@brief Allocates a new copy of our custom saved move
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UARTCharacterMovementComponent();

	//movespeed stuffs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float ADSSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
	float BlockingSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	float AttackingMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool IsSprinting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
	bool IsAiming;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
	bool IsBlocking;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	bool IsAttacking;

	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;
	uint8 RequestToStartBlocking :1;
	uint8 RequestToStartAttacking :1;
	
	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	//rotate stuffs
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	float GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const;
	
	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();
	
	// Aim Down Sights
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();

	//blocking
	UFUNCTION(BlueprintCallable, Category = "Blocking")
    void StartBlocking();
	UFUNCTION(BlueprintCallable, Category = "Block")
    void StopBlocking();

	//attacking
	UFUNCTION(BlueprintCallable, Category = "Blocking")
    void StartAttacking();
	UFUNCTION(BlueprintCallable, Category = "Block")
    void StopAttacking();
};
