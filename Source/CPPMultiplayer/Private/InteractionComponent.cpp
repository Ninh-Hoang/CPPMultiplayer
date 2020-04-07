// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "BaseCharacter.h"

UInteractionComponent::UInteractionComponent(){
	SetComponentTickEnabled(false);
	InteractionTime = 0.;
	InteractionDistance = 200.;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText){

}

void UInteractionComponent::SetInteractableActionText(const FText& ActionText){

}

void UInteractionComponent::Deactivate(){
	Super::Deactivate();

	for (ABaseCharacter* Interactor : Interactors) {
		EndFocus(Interactor);
		EndInteract(Interactor);
	}

	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ABaseCharacter* Character) const{
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1; 
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::BeginFocus(ABaseCharacter* Character){
	if (!IsActive() || GetOwner() || !Character) {
		return;
	}

	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false);

	if (!GetOwner()->HasAuthority()) {
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass())) {
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp)) {
				Prim->SetRenderCustomDepth(true);
			}
		}
	}
}

void UInteractionComponent::EndFocus(ABaseCharacter* Character){
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true);

	if (!GetOwner()->HasAuthority()) {
		for (auto& VisualComp : GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass())) {
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp)) {
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
}

void UInteractionComponent::Interact(ABaseCharacter* Character){
	if (CanInteract(Character)) {
		OnInteract.Broadcast(Character);
	}
}

void UInteractionComponent::BeginInteract(ABaseCharacter* Character){
	if (CanInteract(Character)) {
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ABaseCharacter* Character){
	if (CanInteract(Character)) {
		Interactors.RemoveSingle(Character);
		OnEndInteract.Broadcast(Character);
	}
}

float UInteractionComponent::GetInteractPercentage(){
	if (Interactors.IsValidIndex(0)) {
		if (ABaseCharacter* Interactor = Interactors[0]) {
			if (Interactor && Interactor->IsInteracting()) {
				return 1. - FMath::Abs(Interactor->GetRemainingInteractionTime() / InteractionTime);
			}
		}
	}
	return 0.;
}

void UInteractionComponent::RefreshWidget(){

}
