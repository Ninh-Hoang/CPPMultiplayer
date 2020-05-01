// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */

class FText;
class ABaseCharacter;
class UInventoryComponent;

UCLASS()
class ART_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowIngameUI();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNotification(const FText& Message);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientShowNotification(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeathScreen(ABaseCharacter* Killer);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLootMenu(const UInventoryComponent* LootSource);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideLootMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitPlayer();

	

};
