// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ARTCharacter/ARTCharacterBase.h"
#include "ARTPlayerController.generated.h"

/**
 * 
 */

class UPaperSprite;

UCLASS()
class ART_API AARTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CreateHUD();

	class UARTHUDWidget* GetGSHUD();
	

	/**
	* Weapon HUD info
	*/

	UFUNCTION(BlueprintCallable, Category = "GASShooter|UI")
	void SetHUDReticle(TSubclassOf<class UARTHUDReticle> ReticleClass);

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AARTCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	void ShowDamageNumber_Implementation(float DamageAmount, AARTCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	bool ShowDamageNumber_Validate(float DamageAmount, AARTCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UARTHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASShooter|UI")
		class UARTHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(Exec)
	void Kill();

	UFUNCTION(Server, Reliable)
	void ServerKill();
	void ServerKill_Implementation();
	bool ServerKill_Validate();
};
