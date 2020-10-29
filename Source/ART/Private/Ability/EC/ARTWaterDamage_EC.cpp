// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/EC/ARTWaterDamage_EC.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include <AbilitySystemBlueprintLibrary.h>

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct ARTWaterDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);

	DECLARE_ATTRIBUTE_CAPTUREDEF(WaterBonus);

	DECLARE_ATTRIBUTE_CAPTUREDEF(WaterRes);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);

	// Meta attribute that we're passing into the ExecCalc via SetByCaller on the GESpec so we don't capture it.
	// We still need to declare and define it so that we can output to it.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	ARTWaterDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// here could be like AttackPower attributes that you might want.

		// Capture the Source's Attack. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, AttackPower, Source, false);

		// Capture the Source's WaterBonus. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, WaterBonus, Source, false);

		// Capture the Target's WaterRes. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, WaterRes, Target, false);

		// Capture the Target's Shield. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, Shield, Target, false);

		// The Target's received Damage. This is the value of health that will be subtracted on the Target. We're not capturing this.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, Damage, Target, false);
	}
};

static const ARTWaterDamageStatics& DamageStatics()
{
	static ARTWaterDamageStatics DStatics;
	return DStatics;
}

UARTWaterDamage_EC::UARTWaterDamage_EC()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().WaterBonusDef);
	RelevantAttributesToCapture.Add(DamageStatics().WaterResDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
}

void UARTWaterDamage_EC::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);
	AttackPower = FMath::Max<float>(AttackPower, 0.0f);

	float WaterBonus = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().WaterBonusDef, EvaluationParameters, WaterBonus);

	float WaterRes = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().WaterResDef, EvaluationParameters, WaterRes);

	float Shield = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvaluationParameters, Shield);
	Shield = FMath::Max<float>(Shield, 0.0f);

	// SetByCaller Damage
	float Damage = FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Water")), false, -1.0f), 0.0f);

	float BaseDamage = Damage * AttackPower;

	float UnmitigatedDamage = BaseDamage * (WaterBonus + 1); // Can multiply any damage boosters here

	//formular: only health is under armor mitigation from damage
	float MitigatedDamage = UnmitigatedDamage;

	//if Damage exceed shield, calculate damage to health with armor modification
	if (MitigatedDamage > Shield) {
		MitigatedDamage = Shield + (MitigatedDamage - Shield) * (1 - WaterRes);
	}

	if (MitigatedDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));

		//send event to target that they just took Water damage
		FGameplayEventData EventData;
		EventData.Instigator = SourceActor;
		EventData.Target = TargetActor;
		EventData.EventMagnitude = BaseDamage;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, FGameplayTag::RequestGameplayTag(FName("Data.Damage.Water"), false), EventData);
	}
}
