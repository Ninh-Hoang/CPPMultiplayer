// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ART/ART.h"
#include "GameplayEffectTypes.h"
#include "ARTCharacterBase.generated.h"

class AWeapon;

UCLASS()
class ART_API AARTCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARTCharacterBase(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* AzimuthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* InventoryComponent;

	FGameplayTag CurrentWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|Inventory")
	AWeapon* GetCurrentWeapon() const;

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

protected:

	//WEAPON STUFFS
	bool bChangedWeaponLocally;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	AWeapon* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|Inventory")
	TArray<TSubclassOf<AWeapon>> DefaultInventoryWeaponClasses;

	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// The autonomous client can use this to request the proper CurrentWeapon from the server when it knows it may be
	// out of sync with it from predictive client-side changes.
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerSyncCurrentWeapon();

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// Use this function to manually sync the autonomous client's CurrentWeapon when we're ready to.
	// This allows us to predict weapon changes (changing weapons fast multiple times in a row so that the server doesn't
	// replicate and clobber our CurrentWeapon).
	UFUNCTION(Client, WithValidation, Reliable)
	void ClientSyncCurrentWeapon(AWeapon* InWeapon);


	// Unequips the specified weapon. Used when OnRep_CurrentWeapon fires.
	void UnEquipWeapon(AWeapon* WeaponToUnEquip);

	void UnEquipCurrentWeapon();

	// Server spawns default inventory
	void SpawnDefaultInventory();


	// Cache tags
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	//ABILITY SYSTEM STUFFS
	UPROPERTY()
	class UARTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UARTCharacterAttributeSet* AttributeSetBase;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UARTGameplayAbility>> CharacterAbilities;

	// These effects are only applied one time on startup, Health regen, stamina regen...etc
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EARTAbilityInputID AbilityID) const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetCharacterLevel() const;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);

	//Aim
	FTimerHandle AimTimerHandler;
	void StartAim();
	void StopAim();
	void LookAtCursor();

	//crouch
	void BeginCrouch();
	void EndCrouch();	

	//ONLY USE THIS FOR RE/SPAWNING
	virtual void SetShield(float Shield);
	virtual void SetHealth(float Health);
	virtual void SetStamina(float Stamina);

	// Client only
	virtual void OnRep_PlayerState() override;

	//bind ASC input
	bool ASCInputBound = false;
	void BindASCInput();

	virtual void Restart() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetShieldRegen() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetHealthRegen() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStaminaRegen() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMoveSpeed() const;

	bool IsAlive() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
