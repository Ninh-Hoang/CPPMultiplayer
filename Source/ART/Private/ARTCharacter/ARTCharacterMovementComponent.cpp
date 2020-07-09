// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTCharacterMovementComponent.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include <AbilitySystemComponent.h>
#include "GameplayTagContainer.h"

UARTCharacterMovementComponent::UARTCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.7f;
	ADSSpeedMultiplier = 0.5f;
}

float UARTCharacterMovementComponent::GetMaxSpeed() const
{
	AARTCharacterBase* Owner = Cast<AARTCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	/*if (!Owner->IsAlive())
	{
		return 0.0f;
	}*/

	/*if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
		return 0.0f;
	}*/

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (RequestToStartADS)
	{
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}

void UARTCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UARTCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UARTCharacterMovementComponent* MutableThis = const_cast<UARTCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FARTNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UARTCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UARTCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

//rotate stuffs
FRotator UARTCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	float YawRotateRate = 0.0f;

	AARTCharacterBase* Owner = Cast<AARTCharacterBase>(GetOwner());
	if (Owner) {
		YawRotateRate = Owner->GetRotateRate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		YawRotateRate = RotationRate.Yaw;
	}

	return FRotator(GetAxisDeltaRotation(RotationRate.Pitch, DeltaTime), GetAxisDeltaRotation(YawRotateRate, DeltaTime), GetAxisDeltaRotation(RotationRate.Roll, DeltaTime));
}

float UARTCharacterMovementComponent::GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const
{
	return (InAxisRotationRate >= 0.f) ? (InAxisRotationRate * DeltaTime) : 360.f;
}

//aim stuffs
void UARTCharacterMovementComponent::StartAimDownSights()
{
	RequestToStartADS = true;
}

void UARTCharacterMovementComponent::StopAimDownSights()
{
	RequestToStartADS = false;
}

void UARTCharacterMovementComponent::FARTSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartADS = false;
}

uint8 UARTCharacterMovementComponent::FARTSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool UARTCharacterMovementComponent::FARTSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FARTSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartADS != ((FARTSavedMove*)&NewMove)->SavedRequestToStartADS)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UARTCharacterMovementComponent::FARTSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UARTCharacterMovementComponent* CharacterMovement = Cast<UARTCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartADS = CharacterMovement->RequestToStartADS;
	}
}

void UARTCharacterMovementComponent::FARTSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UARTCharacterMovementComponent* CharacterMovement = Cast<UARTCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UARTCharacterMovementComponent::FARTNetworkPredictionData_Client::FARTNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UARTCharacterMovementComponent::FARTNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FARTSavedMove());
}