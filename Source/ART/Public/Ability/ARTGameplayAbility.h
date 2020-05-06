// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ART/ART.h"
#include "ARTGameplayAbility.generated.h"

/**
 * 
 */
class USkeletalMeshComponent;

USTRUCT()
struct FAbilityMeshMontage
{
	GENERATED_BODY()

public:
	UPROPERTY()
		class USkeletalMeshComponent* Mesh;

	UPROPERTY()
		class UAnimMontage* Montage;

	FAbilityMeshMontage() : Mesh(nullptr), Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* InMesh, class UAnimMontage* InMontage)
		: Mesh(InMesh), Montage(InMontage)
	{
	}
};

UCLASS()
class ART_API UARTGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UARTGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EARTAbilityInputID AbilityInputID = EARTAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EARTAbilityInputID AbilityID = EARTAbilityInputID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
