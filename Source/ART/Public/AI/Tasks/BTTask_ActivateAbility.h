// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/BTTask_GameplayTagBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BTTask_ActivateAbility.generated.h"

/**
 * Activate Gameplay Ability
 */
UCLASS()
class ART_API UBTTask_ActivateAbility : public UBTTask_GameplayTagBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = Node, EditAnywhere)
	bool InstantExecute;

	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;

	FDelegateHandle OnAbilityEndHandle;
	UAbilitySystemComponent* ASC;

	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& Data);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

protected:

};
