// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InteractionWidget.h"
#include "Player/InteractionComponent.h"

void UInteractionWidget::UpdateInteractionWidget(UInteractionComponent* InteractionComponent){
	OwningInteractionComponent = InteractionComponent;
	OnUpdateInteractionWidget();
}

