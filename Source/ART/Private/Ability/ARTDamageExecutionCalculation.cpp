// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ARTDamageExecutionCalculation.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"
#include "Ability/ARTAbilitySystemComponent.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct ARTDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);

	// Meta attribute that we're passing into the ExecCalc via SetByCaller on the GESpec so we don't capture it.
	// We still need to declare and define it so that we can output to it.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	ARTDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// here could be like AttackPower attributes that you might want.

		// Capture the Source's Shield. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, AttackPower, Source, false);

		// Capture the Target's Shield. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, Armor, Target, false);

		// Capture the Target's Shield. Don't snapshot (the false).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, Shield, Target, false);

		// The Target's received Damage. This is the value of health that will be subtracted on the Target. We're not capturing this.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UARTCharacterAttributeSet, Damage, Target, false);
	}
};

static const ARTDamageStatics& DamageStatics()
{
	static ARTDamageStatics DStatics;
	return DStatics;
}

UARTDamageExecutionCalculation::UARTDamageExecutionCalculation()
{
	CritMultiplier = 1.5f;

	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
}

void UARTDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float AttackPower = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);
	AttackPower = FMath::Max<float>(AttackPower, 0.0f);

	float Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);

	float Shield = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvaluationParameters, Shield);
	Shield = FMath::Max<float>(Shield, 0.0f);

	// SetByCaller Damage
	float Damage = FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	float UnmitigatedDamage = Damage * AttackPower; // Can multiply any damage boosters here

	// Check for crit. There's only one character mesh here, but you could have a function on your Character class to return the head bone name
	const FHitResult* Hit = Spec.GetContext().GetHitResult();
	if (AssetTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Effect.Damage.CanCrit"))) && Hit && Hit->BoneName == "b_head")
	{
		UnmitigatedDamage *= CritMultiplier;
		FGameplayEffectSpec* MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
		MutableSpec->DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Effect.Damage.Crit")));
	}

	//formular: only health is under armor mitigation from damage
	float MitigatedDamage = UnmitigatedDamage;

	//if Damage exceed shield, calculate damage to health with armor modification
	if (MitigatedDamage > Shield) {
		MitigatedDamage = Shield + (MitigatedDamage - Shield) * (100 / (Armor + 100));
	}

	if (MitigatedDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));
	}
}
