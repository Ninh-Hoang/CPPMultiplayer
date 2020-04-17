// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BasePlayerController.h"

ABasePlayerController::ABasePlayerController(){
	bReplicates = true;
	bReplicateMovement = true;
}

void ABasePlayerController::ClientShowNotification_Implementation(const FText& Message){
	ShowNotification(Message);
}
