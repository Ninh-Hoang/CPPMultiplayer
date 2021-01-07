// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "GenericTeamAgentInterface.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"

#include "ARTCharacterAI.generated.h"


/**
 * 
 */
UCLASS()
class ART_API AARTCharacterAI : public AARTCharacterBase
{
	GENERATED_BODY()

public:
	//INITIALIZATION
	AARTCharacterAI(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ART|Abilities")
	class TSubclassOf<UARTAttributeSetBase> AttributeSetClass =  UARTCharacterAttributeSet::StaticClass();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UARTNavigationInvokerComponent* NavInvoker;

	class UARTAbilitySystemComponent* HardRefAbilitySystemComponent;

	class UARTCharacterAttributeSet* HardRefAttributeSetBase;

	FDelegateHandle HealthChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
