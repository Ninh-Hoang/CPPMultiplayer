// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "Player/BaseCharacter.h"

/*UPlayerAttributeSet::UPlayerAttributeSet(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer){
	MaxHealth = 1000;
	Health = MaxHealth;
}

void UPlayerAttributeSet::OnRep_Health(){

}

void UPlayerAttributeSet::OnRep_MaxHealth(){

}

void UPlayerAttributeSet::OnRep_AttackPower(){

}

void UPlayerAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const{
	if (Attribute == GetMaxHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth);
	}
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) const{

}

AController* UPlayerAttributeSet::GetSourceActor(const struct FGameplayEffectModCallbackData& Data){
	UAbilitySystemComponent* Source = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

	AActor* AttackingActor = nullptr;
	AController* AttackingController = nullptr;
	AController* AttackingPlayerController = nullptr;

	if (Source && Source->AbilityActorInfo && Source->AbilityActorInfo->AvatarActor) {
		AttackingActor = Source->AbilityActorInfo->AvatarActor.Get();
		AttackingController = Source->AbilityActorInfo->PlayerController.Get();
		if (!AttackingController && AttackingActor) {
			APawn* Pawn = Cast<APawn>(AttackingActor);
			if (Pawn) {
				AttackingController = Pawn->GetController();
			}
		}
	}
	return AttackingController;
}

AActor* UPlayerAttributeSet::GetTargetActor(const struct FGameplayEffectModCallbackData& Data){

}

FGameplayAttribute UPlayerAttributeSet::GetHealthAttribute(){

}

FGameplayAttribute UPlayerAttributeSet::GetMaxHealthAttribute(){

}*/
