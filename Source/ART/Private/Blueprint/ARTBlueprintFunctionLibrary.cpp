// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprint/ARTBlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Ability/ARTGameplayAbility.h"
#include "Ability/ARTGameplayEffectTypes.h"
#include "Ability/ARTGameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Ability/ARTGameplayEffectUIData.h>
#include <Ability/ARTGameplayAbilityUIData.h>


FString UARTBlueprintFunctionLibrary::GetPlayerEditorWindowRole(UWorld* World)
{
	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	return Prefix;
}

UARTGameplayAbility* UARTBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromHandle(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UARTGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UARTGameplayAbility* UARTBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromClass(
	UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass);
		if (AbilitySpec)
		{
			return Cast<UARTGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

bool UARTBlueprintFunctionLibrary::IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent,
                                                                  FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UARTGameplayAbility>(AbilitySpec->GetPrimaryInstance())->IsActive();
		}
	}

	return false;
}

bool UARTBlueprintFunctionLibrary::IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle)
{
	return Handle.IsValid();
}

bool UARTBlueprintFunctionLibrary::DoesEffectContainerSpecHaveEffects(
	const FARTGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UARTBlueprintFunctionLibrary::DoesEffectContainerSpecHaveTargets(
	const FARTGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

void UARTBlueprintFunctionLibrary::ClearEffectContainerSpecTargets(FARTGameplayEffectContainerSpec& ContainerSpec)
{
	ContainerSpec.ClearTargets();
}

void UARTBlueprintFunctionLibrary::AddTargetsToEffectContainerSpec(FARTGameplayEffectContainerSpec& ContainerSpec,
                                                                   const TArray<FGameplayAbilityTargetDataHandle>&
                                                                   TargetData, const TArray<FHitResult>& HitResults,
                                                                   const TArray<AActor*>& TargetActors)
{
	ContainerSpec.AddTargets(TargetData, HitResults, TargetActors);
}

TArray<FActiveGameplayEffectHandle> UARTBlueprintFunctionLibrary::ApplyExternalEffectContainerSpec(
	const FARTGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				//if instigator ASC still alive
				if(SpecHandle.Data.Get()->GetContext().GetInstigatorAbilitySystemComponent()){
					AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
				}
				//TODO: Optimize this loop
				//apply gameplay effect to self instead
				else
				{
					for(TWeakObjectPtr<AActor> Actor : Data->GetActors())
					{
						if(UAbilitySystemComponent* ASC = GetAbilitySystemComponent(Actor.Get()))
						{
							AllEffects.Add(ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()));
						}
					}
				}
			}
		}
	}
	return AllEffects;
}

FGameplayAbilityTargetDataHandle UARTBlueprintFunctionLibrary::EffectContextGetTargetData(
	FGameplayEffectContextHandle EffectContextHandle)
{
	FARTGameplayEffectContext* ARTContext = static_cast<FARTGameplayEffectContext*>(EffectContextHandle.Get());
	if (ARTContext || ARTContext->GetTargetData())
	{
		return *ARTContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}

void UARTBlueprintFunctionLibrary::EffectContextAddTargetData(FGameplayEffectContextHandle EffectContext,
                                                              const FGameplayAbilityTargetDataHandle& TargetData,
                                                              bool Reset)
{
	FARTGameplayEffectContext* ARTContext = static_cast<FARTGameplayEffectContext*>(EffectContext.Get());
	if (ARTContext)
	{
		ARTContext->AddTargetData(TargetData, Reset);
	}
}

float UARTBlueprintFunctionLibrary::EffectContextGetSourceLevel(FGameplayEffectContextHandle EffectContext)
{
	FARTGameplayEffectContext* ARTContext = static_cast<FARTGameplayEffectContext*>(EffectContext.Get());
	if (ARTContext)
	{
		return ARTContext->GetSourceLevel();
	}
	return 0.0f;
}

void UARTBlueprintFunctionLibrary::ClearTargetData(FGameplayAbilityTargetDataHandle& TargetData)
{
	TargetData.Clear();
}

FGameplayTargetDataFilterHandle UARTBlueprintFunctionLibrary::MakeTargetDataFilterByActorType(AActor* FilterActor,
	AActor* InSourceActor, TEnumAsByte<EARTTargetSelectionFilter::Type> InTargetTypeFilter,
	TEnumAsByte<ETargetDataFilterSelf::Type> InSelfFilter, TSubclassOf<AActor> InRequiredActorClass,
	bool InReverseFilter)
{
	FARTTargetFilter Filter;
	Filter.SourceActor = InSourceActor;
	Filter.ActorTypeFilter = InTargetTypeFilter;
	Filter.SelfFilter = InSelfFilter;
	Filter.RequiredActorClass = InRequiredActorClass;
	Filter.bReverseFilter = InReverseFilter;

	FGameplayTargetDataFilter* NewFilter = new FARTTargetFilter(Filter);
	NewFilter->InitializeFilterContext(FilterActor);

	FGameplayTargetDataFilterHandle FilterHandle;
	FilterHandle.Filter = TSharedPtr<FGameplayTargetDataFilter>(NewFilter);
	return FilterHandle;
}

FGameplayTargetDataFilterHandle UARTBlueprintFunctionLibrary::MakeTargetDataFilterByTeamAttitude(AActor* FilterActor,
	AActor* InSourceActor, TEnumAsByte<ETeamAttitude::Type> InTeamAttitude,
	TEnumAsByte<ETargetDataFilterSelf::Type> InSelfFilter,
	TSubclassOf<AActor> InRequiredActorClass, bool InReverseFilter)
{
	FARTTargetFilterTeamID Filter;
	Filter.SourceActor = InSourceActor;
	Filter.TeamAttitude = InTeamAttitude;
	Filter.SelfFilter = InSelfFilter;
	Filter.RequiredActorClass = InRequiredActorClass;
	Filter.bReverseFilter = InReverseFilter;

	FGameplayTargetDataFilter* NewFilter = new FARTTargetFilterTeamID(Filter);
	NewFilter->InitializeFilterContext(FilterActor);

	FGameplayTargetDataFilterHandle FilterHandle;
	FilterHandle.Filter = TSharedPtr<FGameplayTargetDataFilter>(NewFilter);
	return FilterHandle;
}

TArray<FGameplayAbilityTargetDataHandle> UARTBlueprintFunctionLibrary::FilterTargetDataArray(
	TArray<FGameplayAbilityTargetDataHandle> TargetDataArray, FGameplayTargetDataFilterHandle Filterhandle)
{
	TArray<FGameplayAbilityTargetDataHandle> OutTargetDataArray;

	for (int i = 0; i < TargetDataArray.Num(); i++)
	{
		OutTargetDataArray.Add(UAbilitySystemBlueprintLibrary::FilterTargetData(TargetDataArray[i], Filterhandle));
	}

	return OutTargetDataArray;
}

FGameplayAbilityTargetDataHandle UARTBlueprintFunctionLibrary::MakeTargetDataFromHitArray(TArray<FHitResult>& HitResults)
{
	FGameplayAbilityTargetDataHandle ReturnDataHandle;

	for (FHitResult& Hit : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
		ReturnDataHandle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
	}
	return ReturnDataHandle;
}

TArray<FGameplayAbilityTargetDataHandle> UARTBlueprintFunctionLibrary::MakeArrayTargetDataFromHitArray(TArray<FHitResult>& HitResults)
{
	TArray<FGameplayAbilityTargetDataHandle> ReturnDataHandles;

	for (FHitResult& Hit : HitResults)
	{
		ReturnDataHandles.Add(AbilityTargetDataFromHitResult(Hit));
	}
	return ReturnDataHandles;
}

void UARTBlueprintFunctionLibrary::InitializePropertyMap(FGameplayTagBlueprintPropertyMap& InMap, UObject* Owner,
                                                         UAbilitySystemComponent* ASC)
{
}

float UARTBlueprintFunctionLibrary::GetTagCallerMag(UAbilitySystemComponent* InASC,
                                                    FActiveGameplayEffectHandle& InActiveHandle, FGameplayTag CallerTag)
{
	if (InASC && InActiveHandle.IsValid())
	{
		const FActiveGameplayEffect* ActiveGE = InASC->GetActiveGameplayEffect(InActiveHandle);
		return ActiveGE->Spec.GetSetByCallerMagnitude(CallerTag);
	}
	return 0.0f;
}

UARTGameplayEffectUIData* UARTBlueprintFunctionLibrary::GetGameplayEffectUIDataFromActiveHandle(
	const FActiveGameplayEffectHandle& InActiveHandle)
{
	if (InActiveHandle.IsValid())
	{
		if (const UAbilitySystemComponent* ASC = InActiveHandle.GetOwningAbilitySystemComponent())
		{
			const FActiveGameplayEffect* ActiveEffect = ASC->GetActiveGameplayEffect(InActiveHandle);
			UGameplayEffectUIData* Data = ActiveEffect->Spec.Def->UIData;
			return Cast<UARTGameplayEffectUIData>(Data);
		}
	}
	return nullptr;
}

UARTGameplayAbilityUIData* UARTBlueprintFunctionLibrary::GetGameplayAbilityUIDataFromInput(
	UAbilitySystemComponent* InASC, const EARTAbilityInputID Input)
{
	if (InASC)
	{
		if(FGameplayAbilitySpec* Spec = InASC->FindAbilitySpecFromInputID(static_cast<int32>(Input)))
		{
			UARTGameplayAbility* Ability = Cast<UARTGameplayAbility>(Spec->Ability);
			return Cast<UARTGameplayAbilityUIData>(Ability->UIData);
			
		}
	}
	return nullptr;
}
