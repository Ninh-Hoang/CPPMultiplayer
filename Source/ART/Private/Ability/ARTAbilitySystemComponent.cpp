// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ARTAbilitySystemComponent.h"

void UARTAbilitySystemComponent::ReceiveDamage(UARTAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
