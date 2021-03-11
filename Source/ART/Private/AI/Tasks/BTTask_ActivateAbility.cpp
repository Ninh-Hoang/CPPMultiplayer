// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ActivateAbility.h"

#include "Ability/ARTGameplayAbility.h"
#include "ARTCharacter/ARTCharacterBase.h"

UBTTask_ActivateAbility::UBTTask_ActivateAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Activate Gameplay Ability";
	bCreateNodeInstance = true;
}


EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AController* MyController = Cast<AController>(OwnerComp.GetOwner());
	APawn* MyPawn = MyController ? MyController->GetPawn() : NULL;

	if(!MyPawn)
	{
		return EBTNodeResult::Failed;
	}
	if(AARTCharacterBase* AvatarActor = Cast<AARTCharacterBase>(MyPawn))
	{
		bool Activated = AvatarActor->ActivateAbilitiesWithTags(GameplayTagContainer, false);
		
		if(!Activated) return EBTNodeResult::Failed;
		if(InstantExecute) return EBTNodeResult::Succeeded;

		ASC = AvatarActor->GetAbilitySystemComponent();
		OnAbilityEndHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateAbility::OnAbilityEnded);

		MyOwnerComp = &OwnerComp;
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASC->OnAbilityEnded.Remove(OnAbilityEndHandle);
	return EBTNodeResult::Aborted;
}

void UBTTask_ActivateAbility::OnAbilityEnded(const FAbilityEndedData& Data)
{
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
	const EBTNodeResult::Type NodeResult = Data.bWasCancelled?  EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	FinishLatentTask(*OwnerComp, NodeResult);
}

FString UBTTask_ActivateAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Activate Ability that: \n %s "), *CachedDescription);
}

#if WITH_EDITOR
FName UBTTask_ActivateAbility::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Task.Icon");
}

void UBTTask_ActivateAbility::BuildDescription()
{
	CachedDescription = GameplayTagContainer.ToMatchingText(EGameplayContainerMatchType::All, false).ToString();
}
 
 void UBTTask_ActivateAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property == NULL)
	{
		return;
	}

	BuildDescription();
}
#endif	// WITH_EDITOR