// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ARTGameplayAbility.h"
#include <AbilitySystemComponent.h>
#include "ARTCharacter/ARTCharacterBase.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "Blueprint/ARTTargetType.h"
#include "ARTCharacter/ARTSurvivor.h"
#include "GameplayTagContainer.h"
#include "Ability/ARTAbilitySystemGlobals.h"
#include <ARTCharacter/ARTPlayerController.h>
#include <Weapon/Weapon.h>

UARTGameplayAbility::UARTGameplayAbility()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bActivateAbilityOnGranted = false;
	bAllowRemoteGrantingActivation = false;
	bActivateOnInput = true;
	bSourceObjectMustEqualCurrentWeaponToActivate = false;
	bCannotActivateWhileInteracting = true;

	InteractingTag = FGameplayTag::RequestGameplayTag("State.Interacting");
	InteractingRemovalTag = FGameplayTag::RequestGameplayTag("State.InteractingRemoval");
}

void UARTGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                     bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	AbilityEnd.Broadcast(bWasCancelled);
}

void UARTGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	
	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = AbilitySystemComponent->TryActivateAbility(
			Spec.Handle, bAllowRemoteGrantingActivation);
	}

	//for charged ability
	if (AbilityCharge > 1)
	{
		CurrentCharges = AbilityCharge;
	}

	const FGameplayTagContainer* CDTags = GetCooldownTags();
	if (CDTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CDTags->GetByIndex(0), EGameplayTagEventType::AnyCountChange).
		                        AddUObject(this, &UARTGameplayAbility::OnCooldownTagEventCallback);
	}

	//re-check ability trigger when given, this is needed if the ability is given via a GameplayEffect and need to recheck trigger conditions
	const TArray<FAbilityTriggerData>& AbilityTriggerData = AbilityTriggers;

	for (const FAbilityTriggerData& TriggerData : AbilityTriggerData)
	{
		FGameplayTag EventTag = TriggerData.TriggerTag;

		if (TriggerData.TriggerSource != EGameplayAbilityTriggerSource::GameplayEvent)
		{
			if (AbilitySystemComponent->GetTagCount(TriggerData.TriggerTag))
			{
				bool ActivatedAbility = AbilitySystemComponent->TryActivateAbility(
					Spec.Handle, bAllowRemoteGrantingActivation);
			}
		}
	}
}

void UARTGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* const ASC = GetAbilitySystemComponentFromActorInfo();
	
	//TODO: Should we register on ActivatedAbility or granted?
	//for listen to ASC tag and cancel itself if match AbilityCancelTag
	TArray<FGameplayTag> CancelTagArray;
	AbilityCancelTag.GetGameplayTagArray(CancelTagArray);

	for (FGameplayTag CancelTag : CancelTagArray)
	{
		ASC->RegisterGameplayTagEvent(CancelTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UARTGameplayAbility::OnCancelTagEventCallback);
	}
}

FGameplayAbilityTargetDataHandle UARTGameplayAbility::MakeGameplayAbilityTargetDataHandleFromActorArray(
	const TArray<AActor*> TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

FGameplayAbilityTargetDataHandle UARTGameplayAbility::MakeGameplayAbilityTargetDataHandleFromHitResults(
	const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FARTGameplayEffectContainerSpec UARTGameplayAbility::MakeEffectContainerSpecFromContainer(
	const FARTGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	// First figure out our actor info
	FARTGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AARTCharacterBase* AvatarCharacter = Cast<AARTCharacterBase>(AvatarActor);
	UARTAbilitySystemComponent* OwningASC = UARTAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			TArray<FGameplayAbilityTargetDataHandle> TargetData;
			const UARTTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			TargetTypeCDO->GetTargets(AvatarCharacter, AvatarActor, EventData, TargetData, HitResults, TargetActors);
			ReturnSpec.AddTargets(TargetData, HitResults, TargetActors);
		}

		// If we don't have an override level, use the ability level
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			//OverrideGameplayLevel = OwningASC->GetDefaultAbilityLevel();
			OverrideGameplayLevel = GetAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.
			           Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FARTGameplayEffectContainerSpec UARTGameplayAbility::MakeEffectContainerSpec(
	FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FARTGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FARTGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UARTGameplayAbility::ApplyEffectContainerSpec(
	const FARTGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

UObject* UARTGameplayAbility::K2_GetSourceObject(FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo& ActorInfo) const
{
	return GetSourceObject(Handle, &ActorInfo);
}

bool UARTGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle,
                                                     bool EndAbilityImmediately)
{
	UARTAbilitySystemComponent* ASC = Cast<UARTAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC)
	{
		return ASC->BatchRPCTryActivateAbility(InAbilityHandle, EndAbilityImmediately);
	}

	return false;
}

void UARTGameplayAbility::ExternalEndAbility()
{
	check(CurrentActorInfo);

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FString UARTGameplayAbility::GetCurrentPredictionKeyStatus()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	return ASC->ScopedPredictionKey.ToString() + " is valid for more prediction: " + (
		ASC->ScopedPredictionKey.IsValidForMorePrediction() ? TEXT("true") : TEXT("false"));
}

bool UARTGameplayAbility::IsPredictionKeyValidForMorePrediction() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	return ASC->ScopedPredictionKey.IsValidForMorePrediction();
}

bool UARTGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayTagContainer* SourceTAART,
                                             const FGameplayTagContainer* TargetTAART,
                                             OUT FGameplayTagContainer* OptionalRelevantTAART) const
{
	if (bCannotActivateWhileInteracting)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC->GetTagCount(InteractingTag) > ASC->GetTagCount(InteractingRemovalTag))
		{
			return false;
		}
	}

	if (bSourceObjectMustEqualCurrentWeaponToActivate)
	{
		AARTSurvivor* Hero = Cast<AARTSurvivor>(ActorInfo->AvatarActor);

		if (Hero && Hero->GetCurrentWeapon() && static_cast<UObject*>(Hero->GetCurrentWeapon()) == GetSourceObject(
			Handle, ActorInfo))
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTAART, TargetTAART, OptionalRelevantTAART);
		}
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTAART, TargetTAART, OptionalRelevantTAART);
}

bool UARTGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    OUT FGameplayTagContainer* OptionalRelevantTAART) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTAART) && ARTCheckCost(Handle, *ActorInfo);
}

void UARTGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown")),
		                                               CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UARTGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        OUT FGameplayTagContainer* OptionalRelevantTags /* = nullptr */) const
{
	if (AbilityCharge > 1)
	{
		const FGameplayTagContainer* CDTags = GetCooldownTags();

		if (CDTags)
		{
			if (CurrentCharges == 0)
			{
				const FGameplayTag& FailCDTag = UAbilitySystemGlobals::Get().ActivateFailCooldownTag;

				if (OptionalRelevantTags && FailCDTag.IsValid())
				{
					OptionalRelevantTags->AddTag(FailCDTag);
				}

				return false;
			}
		}
		return true;
	}

	return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

const FGameplayTagContainer* UARTGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UARTGameplayAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (CurrentCharges > 0)
	{
		CurrentCharges -= 1;
	}
	Super::CommitExecute(Handle, ActorInfo, ActivationInfo);
}

void UARTGameplayAbility::OnCooldownTagEventCallback(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (AbilityCharge > 1)
	{
		const FGameplayTagContainer* CDTags = GetCooldownTags();
		UAbilitySystemComponent* const AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CDTags);
		CurrentCharges = AbilityCharge - AbilitySystemComponent->GetAggregatedStackCount(Query);
	}
}

void UARTGameplayAbility::OnCancelTagEventCallback(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount>0) 
	{
		UAbilitySystemComponent* const ASC = GetAbilitySystemComponentFromActorInfo();
		//for listen to ASC tag and cancel itself if match AbilityCancelTag
		TArray<FGameplayTag> CancelTagArray;
		AbilityCancelTag.GetGameplayTagArray(CancelTagArray);

		for (const FGameplayTag CancelTag : CancelTagArray)
		{
			ASC->RegisterGameplayTagEvent(CancelTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
		
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

bool UARTGameplayAbility::ARTCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo& ActorInfo) const
{
	return true;
}

void UARTGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo) const
{
	ARTApplyCost(Handle, *ActorInfo, ActivationInfo);
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

int32 UARTGameplayAbility::GetCurrentCharge()
{
	return CurrentCharges;
}

void UARTGameplayAbility::SetHUDReticle(TSubclassOf<UARTHUDReticle> ReticleClass)
{
	AARTPlayerController* PC = Cast<AARTPlayerController>(CurrentActorInfo->PlayerController);
	if (PC)
	{
		PC->SetHUDReticle(ReticleClass);
	}
}

void UARTGameplayAbility::ResetHUDReticle()
{
	AARTPlayerController* PC = Cast<AARTPlayerController>(CurrentActorInfo->PlayerController);
	if (PC)
	{
		AARTSurvivor* Survivor = Cast<AARTSurvivor>(CurrentActorInfo->AvatarActor);
		if (Survivor && Survivor->GetCurrentWeapon())
		{
			PC->SetHUDReticle(Survivor->GetCurrentWeapon()->GetPrimaryHUDReticleClass());
		}
		else
		{
			PC->SetHUDReticle(nullptr);
		}
	}
}

void UARTGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (IsPredictingClient())
	{
		UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
		check(ASC);

		FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

		FGameplayTag ApplicationTag; // Fixme: where would this be useful?
		CurrentActorInfo->AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(), TargetData, ApplicationTag, ASC->ScopedPredictionKey);
	}
}

bool UARTGameplayAbility::IsInputPressed() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}

UAnimMontage* UARTGameplayAbility::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		return AbilityMeshMontage.Montage;
	}

	return nullptr;
}

void UARTGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* InCurrentMontage)
{
	ensure(IsInstantiated());

	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InCurrentMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FAbilityMeshMontage(InMesh, InCurrentMontage));
	}
}

TArray<FActiveGameplayEffectHandle> UARTGameplayAbility::ApplyEffectContainer(
	FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel /*= -1*/)
{
	FARTGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}

bool UARTGameplayAbility::FindAbillityMeshMontage(USkeletalMeshComponent* InMesh,
                                                  FAbilityMeshMontage& InAbilityMeshMontage)
{
	for (FAbilityMeshMontage& MeshMontage : CurrentAbilityMeshMontages)
	{
		if (MeshMontage.Mesh == InMesh)
		{
			InAbilityMeshMontage = MeshMontage;
			return true;
		}
	}

	return false;
}

void UARTGameplayAbility::MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	check(CurrentActorInfo);

	UARTAbilitySystemComponent* const AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(
		GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageJumpToSectionForMesh(InMesh, SectionName);
	}
}

void UARTGameplayAbility::MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName,
                                                           FName ToSectionName)
{
	check(CurrentActorInfo);

	UARTAbilitySystemComponent* const AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(
		GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageSetNextSectionNameForMesh(InMesh, FromSectionName, ToSectionName);
	}
}

void UARTGameplayAbility::MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UARTAbilitySystemComponent* const AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(
		CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		// We should only stop the current montage if we are the animating ability
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
		}
	}
}

void UARTGameplayAbility::MontageStopForAllMeshes(float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UARTAbilitySystemComponent* const AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(
		CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->StopAllCurrentMontages(OverrideBlendOutTime);
		}
	}
}

bool UARTGameplayAbility::AreAbilityTasksActive() const
{
	return ActiveTasks.Num() > 0;
}
