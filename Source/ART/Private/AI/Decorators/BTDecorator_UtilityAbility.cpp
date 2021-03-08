// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTDecorator_UtilityAbility.h"

#include "AIController.h"
#include "Ability/ARTGameplayAbility.h"
#include "ARTCharacter/ARTCharacterBase.h"

UBTDecorator_UtilityAbility::UBTDecorator_UtilityAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Ability Utility";
}

FString UBTDecorator_UtilityAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Get Utility Score from Ability function."));
}

void UBTDecorator_UtilityAbility::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTUtilityAbilityDecoratorMemory* MyMemory = (FBTUtilityAbilityDecoratorMemory*)NodeMemory;
	if (MyMemory->UtilityScore)
	{
		Values.Add(FString::Printf(TEXT("Score: %s"), *FString::SanitizeFloat(MyMemory->UtilityScore)));
	}
}

void UBTDecorator_UtilityAbility::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	FBTUtilityAbilityDecoratorMemory* DecoratorMemory = (FBTUtilityAbilityDecoratorMemory*)NodeMemory;

	DecoratorMemory->UtilityScore = 0.0f;
}

uint16 UBTDecorator_UtilityAbility::GetInstanceMemorySize() const
{
	return sizeof(FBTUtilityAbilityDecoratorMemory);
}

float UBTDecorator_UtilityAbility::CalculateUtilityValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIOwner = OwnerComp.GetAIOwner();
	AARTCharacterBase* Avatar = Cast<AARTCharacterBase>(AIOwner->GetPawn());
	if(Avatar)
	{
		TArray<UARTGameplayAbility*> Abilities;
		Avatar->GetActiveAbilitiesWithTags(GameplayTagContainer, Abilities);
		if(Abilities.Num()>0)
		{
			FBTUtilityAbilityDecoratorMemory* MyMemory = (FBTUtilityAbilityDecoratorMemory*)NodeMemory;
			MyMemory->UtilityScore = Abilities[0]->ScoreAbilityUtility();
			return MyMemory->UtilityScore;
		}
	}
	return 0.0f;
}
