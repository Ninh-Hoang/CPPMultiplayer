// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PlayerAttributeSet.generated.h"

/**
 * 
 */

class AController;
class AActor;

UCLASS()
class ART_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	/*UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "Health")
	float Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_AttackPower, BlueprintReadOnly, Category = "Damage")
	float AttackPower;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_MaxHealth();

	UFUNCTION()
	void OnRep_AttackPower();

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	AController* GetSourceActor(const struct FGameplayEffectModCallbackData& Data);
	AActor* GetTargetActor(const struct FGameplayEffectModCallbackData& Data);

	static FGameplayAttribute GetHealthAttribute();
	static FGameplayAttribute GetMaxHealthAttribute();*/
};
