// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Ability/ARTGameplayAbilityTypes.h"
#include "ARTTargetFilter.h"
#include "ARTBlueprintFunctionLibrary.generated.h"

/**
 * 
 */

UCLASS()
class ART_API UARTBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//returns the player's editor window role - Server (listen host) or Client #

	UFUNCTION(BlueprintCallable)
	static FString GetPlayerEditorWindowRole(UWorld* World);

	/**
	 * Gameplay Ability Stuffs
	 */

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UARTGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent,
	                                                                FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UARTGameplayAbility* GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent,
	                                                               TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static bool IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent,
	                                           FGameplayAbilitySpecHandle Handle);


	/*
	*FGameplayEffectSpecHandle
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayEffectSpec")
	static bool IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle);
	
	/*
	*GameplayEffectContainerSpec
	*/

	// USTRUCTs cannot contain UFUNCTIONS so we make static functions here
	// Checks if spec has any effects
	UFUNCTION(BlueprintPure, Category = "Ability|Container")
	static bool DoesEffectContainerSpecHaveEffects(const FARTGameplayEffectContainerSpec& ContainerSpec);

	// Checks if spec has any targets
	UFUNCTION(BlueprintPure, Category = "Ability|Container")
	static bool DoesEffectContainerSpecHaveTargets(const FARTGameplayEffectContainerSpec& ContainerSpec);

	// Clears spec's targets
	UFUNCTION(BlueprintCallable, Category = "Ability|Container")
	static void ClearEffectContainerSpecTargets(UPARAM(ref) FARTGameplayEffectContainerSpec& ContainerSpec);

	// Adds targets to a copy of the passed in effect container spec and returns it
	UFUNCTION(BlueprintCallable, Category = "Ability|Container", Meta = (AutoCreateRefTerm =
		"TargetData, HitResults, TargetActors"))
	static void AddTargetsToEffectContainerSpec(UPARAM(ref) FARTGameplayEffectContainerSpec& ContainerSpec,
	                                            const TArray<FGameplayAbilityTargetDataHandle>& TargetData,
	                                            const TArray<FHitResult>& HitResults,
	                                            const TArray<AActor*>& TargetActors);

	// Applies container spec that was made from an ability
	UFUNCTION(BlueprintCallable, Category = "Ability|Container")
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(
		const FARTGameplayEffectContainerSpec& ContainerSpec);

	/**
	* FARTGameplayEffectContext
	*/

	// Returns TargetData
	UFUNCTION(BlueprintPure, Category = "Ability|EffectContext", Meta = (DisplayName = "GetTargetData"))
	static FGameplayAbilityTargetDataHandle EffectContextGetTargetData(FGameplayEffectContextHandle EffectContext);

	// Adds TargetData
	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContext", Meta = (DisplayName = "AddTargetData"))
	static void EffectContextAddTargetData(FGameplayEffectContextHandle EffectContext,
	                                       const FGameplayAbilityTargetDataHandle& TargetData, bool Reset);

	// Returns SourceLevel
	UFUNCTION(BlueprintPure, Category = "Ability|EffectContext", Meta = (DisplayName = "GetSourceLevel"))
    static float EffectContextGetSourceLevel(FGameplayEffectContextHandle EffectContext);

	
	/**
	* FGameplayAbilityTargetDataHandle
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static void ClearTargetData(UPARAM(ref) FGameplayAbilityTargetDataHandle& TargetData);

	/*
	* FARTTargetFilterHandle filter by type
	*/
	UFUNCTION(BlueprintPure, Category = "Ability|TargetData", Meta = (DisplayName =
		"Make Target Data Filter by Target Type"))
	static FGameplayTargetDataFilterHandle MakeTargetDataFilterByActorType(AActor* FilterActor, AActor* InSourceActor,
	                                                                       TEnumAsByte<EARTTargetSelectionFilter::Type>
	                                                                       InTargetTypeFilter,
	                                                                       TEnumAsByte<ETargetDataFilterSelf::Type>
	                                                                       InSelfFilter,
	                                                                       TSubclassOf<AActor> InRequiredActorClass,
	                                                                       bool InReverseFilter);

	/*
	* FARTTargetFilterHandle filter by TeamAttitude
	*/
	UFUNCTION(BlueprintPure, Category = "Ability|TargetData", Meta = (DisplayName =
		"Make Target Data Filter by Team Attitude"))
	static FGameplayTargetDataFilterHandle MakeTargetDataFilterByTeamAttitude(
		AActor* FilterActor, AActor* InSourceActor,
		TEnumAsByte<ETeamAttitude::Type> InTeamAttitude,
		TEnumAsByte<ETargetDataFilterSelf::Type> InSelfFilter, TSubclassOf<AActor> InRequiredActorClass,
		bool InReverseFilter);
	/*
	* Filter Bulk TargetData
	*/
	UFUNCTION(BlueprintPure, Category = "Ability|TargetData", Meta = (DisplayName = "Filter Target Data Array"))
	static TArray<FGameplayAbilityTargetDataHandle> FilterTargetDataArray(
		TArray<FGameplayAbilityTargetDataHandle> TargetDataArray,
		FGameplayTargetDataFilterHandle Filterhandle);

	/*
	* turn HitResult in to TargetData
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataFromHit(FHitResult HitResult);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataFromHitArray(TArray<FHitResult> HitResults);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetDataHandle> MakeArrayTargetDataFromHitArray(TArray<FHitResult> HitResults);

	/*
	* GameplayTagBlueprintPropertyMap ultilities
	*/
	static void InitializePropertyMap(FGameplayTagBlueprintPropertyMap& InMap, UObject* Owner,
	                                  UAbilitySystemComponent* ASC);

	/*
	* Get Tag caller Mag
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|ActiveEffect")
	static float GetTagCallerMag(UAbilitySystemComponent* InASC, FActiveGameplayEffectHandle& InActiveHandle,
	                             FGameplayTag CallerTag);

	/*
	* Get UI information from GameplayEffect Handle, or Spec?
	*/

	UFUNCTION(BlueprintCallable, Category = "Ability|ActiveEffect", Meta = (DisplayName =
		"Get GE UI Data from Active Handle"))
	static UARTGameplayEffectUIData* GetGameplayEffectUIDataFromActiveHandle(
		const FActiveGameplayEffectHandle& InActiveHandle);

	/*
	* Get UI information from Ability Input, Spec, or Handle?
	*/

	UFUNCTION(BlueprintCallable, Category = "Ability|ActiveAbility", Meta = (DisplayName = "Get GA UI Data from Input"))
	static UARTGameplayAbilityUIData* GetGameplayAbilityUIDataFromInput(UAbilitySystemComponent* InASC,
	                                                                    const EARTAbilityInputID Input);
};
