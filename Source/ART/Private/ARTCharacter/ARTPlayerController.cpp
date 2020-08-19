// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTPlayerController.h"
#include "ARTCharacter/ARTCharacterAttributeSet.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include <ARTCharacter/ARTPlayerState.h>
#include <GameFramework/Actor.h>
#include <Widget/ARTHUDWidget.h>
#include <Weapon/Weapon.h>
#include <ARTCharacter/ARTSurvivor.h>

void AARTPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AARTPlayerState* PS = GetPlayerState<AARTPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UARTHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentShield(PS->GetShield());
	UIHUDWidget->SetMaxShield(PS->GetMaxShield());
	UIHUDWidget->SetShieldRegen(PS->GetShieldRegen());
	UIHUDWidget->SetShieldPercentage(PS->GetShield() / PS->GetMaxShield());

	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / PS->GetMaxHealth());
	UIHUDWidget->SetHealthRegen(PS->GetHealthRegen());

	UIHUDWidget->SetCurrentEnergy(PS->GetEnergy());
	UIHUDWidget->SetMaxEnergy(PS->GetMaxEnergy());
	UIHUDWidget->SetEnergyPercentage(PS->GetEnergy() / PS->GetMaxEnergy());
	UIHUDWidget->SetEnergyRegen(PS->GetEnergyRegen());

	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / PS->GetMaxStamina());
	UIHUDWidget->SetStaminaRegen(PS->GetStaminaRegen());


	UIHUDWidget->SetSurvivorLevel(PS->GetCharacterLevel());

	AARTSurvivor* Survivor = GetPawn<AARTSurvivor>();
	if (Survivor)
	{
		AWeapon* CurrentWeapon = Survivor->GetCurrentWeapon();
		if (CurrentWeapon)
		{
			//UIHUDWidget->SetEquippedWeaponSprite(CurrentWeapon->PrimaryIcon);
			//UIHUDWidget->SetEquippedWeaponStatusText(CurrentWeapon->GetDefaultStatusText());
			//UIHUDWidget->SetPrimaryClipAmmo(Survivor-->GetPrimaryClipAmmo());
			//UIHUDWidget->SetReticle(CurrentWeapon->GetPrimaryHUDReticleClass());

			// PlayerState's Pawn isn't set up yet so we can't just call PS->GetPrimaryReserveAmmo()
			/*if (PS->GetAmmoAttributeSet())
			{
				FGameplayAttribute Attribute = PS->GetAmmoAttributeSet()->GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType);
				if (Attribute.IsValid())
				{
					UIHUDWidget->SetPrimaryReserveAmmo(PS->GetAbilitySystemComponent()->GetNumericAttribute(Attribute));
				}
			}*/
		}
	}
}

class UARTHUDWidget* AARTPlayerController::GetGSHUD()
{
	return UIHUDWidget;
}

void AARTPlayerController::SetHUDReticle(TSubclassOf<class UARTHUDReticle> ReticleClass)
{
	// !GetWorld()->bIsTearingDown Stops an error when quitting PIE while targeting when the EndAbility resets the HUD reticle
	if (UIHUDWidget && GetWorld() && !GetWorld()->bIsTearingDown)
	{
		UIHUDWidget->SetReticle(ReticleClass);
	}
}

void AARTPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AARTCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags)
{
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->AddDamageNumber(DamageAmount, DamageNumberTags);
	}
}

bool AARTPlayerController::ShowDamageNumber_Validate(float DamageAmount, AARTCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags)
{
	return true;
}

void AARTPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AARTPlayerState* PS = GetPlayerState<AARTPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AARTPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}

void AARTPlayerController::Kill()
{
	ServerKill();
}

void AARTPlayerController::ServerKill_Implementation()
{
	AARTPlayerState* PS = GetPlayerState<AARTPlayerState>();
	if (PS)
	{
		PS->GetAttributeSetBase()->SetHealth(0.0f);
	}
}


bool AARTPlayerController::ServerKill_Validate()
{
	return true;
}
