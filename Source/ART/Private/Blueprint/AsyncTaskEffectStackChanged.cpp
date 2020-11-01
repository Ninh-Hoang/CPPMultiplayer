// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprint/AsyncTaskEffectStackChanged.h"

UAsyncTaskEffectStackChanged* UAsyncTaskEffectStackChanged::ListenForGameplayEffectStackChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer InEffectGameplayTags)
{
	UAsyncTaskEffectStackChanged* ListenForGameplayEffectStackChange = NewObject<UAsyncTaskEffectStackChanged>();
	ListenForGameplayEffectStackChange->ASC = AbilitySystemComponent;
	InEffectGameplayTags.GetGameplayTagArray(ListenForGameplayEffectStackChange->EffectGameplayTags);

	if (!IsValid(AbilitySystemComponent) || InEffectGameplayTags.Num() < 1)
	{
		ListenForGameplayEffectStackChange->EndTask();
		return nullptr;
	}

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForGameplayEffectStackChange, &UAsyncTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(ListenForGameplayEffectStackChange, &UAsyncTaskEffectStackChanged::OnRemoveGameplayEffectCallback);

	return ListenForGameplayEffectStackChange;
}

void UAsyncTaskEffectStackChanged::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UAsyncTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);
	for (FGameplayTag EffectGameplayTag : EffectGameplayTags)
	{
		if (AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
		{
			ASC->OnGameplayEffectStackChangeDelegate(ActiveHandle)->AddUObject(this, &UAsyncTaskEffectStackChanged::GameplayEffectStackChanged);
			OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, ActiveHandle, 1, 0);
		}
	}
}

void UAsyncTaskEffectStackChanged::OnRemoveGameplayEffectCallback(const FActiveGameplayEffect& EffectRemoved)
{
	FGameplayTagContainer AssetTags;
	EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);
	for (FGameplayTag EffectGameplayTag : EffectGameplayTags)
	{
		if (AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
		{
			OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, EffectRemoved.Handle, 0, 1);
		}
	}
}

//TO DO RIGHT NOW THE DEGELATE ONLY BOARDCAST THE FIRST ASSET TAG OF THE EFFECT THAT HAS CHANGED TAG, NOT THE ACTUAL TAG INIT FROM THE CONTAINER
void UAsyncTaskEffectStackChanged::GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	FGameplayTagContainer Tags;
	ASC->GetActiveGameplayEffect(EffectHandle)->Spec.GetAllAssetTags(Tags);
	OnGameplayEffectStackChange.Broadcast(Tags.GetByIndex(0), EffectHandle, NewStackCount, PreviousStackCount);
}