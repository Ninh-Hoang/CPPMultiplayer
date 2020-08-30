// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ART/ART.h"
#include "GameplayEffectTypes.h"
#include <GenericTeamAgentInterface.h>
#include "ARTCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AARTCharacterBase*, Character);

class AWeapon;

USTRUCT(BlueprintType)
struct ART_API FARTDamageNumber
{
	GENERATED_USTRUCT_BODY()

		float DamageAmount;

	FGameplayTagContainer Tags;

	FARTDamageNumber() {}

	FARTDamageNumber(float InDamageAmount, FGameplayTagContainer InTags) : DamageAmount(InDamageAmount)
	{
		// Copy tag container
		Tags.AppendTags(InTags);
	}
};

UCLASS()
class ART_API AARTCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ART|Character")
	FText CharacterName;

	//TEAM STUFFS handle by Interface
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ART|Character")
	int TeamNumber;

	FGenericTeamId GetGenericTeamId() const override;

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void RemoveCharacterAbilities();

	//public loose tag action
	//DIE stuffs

	UPROPERTY(BlueprintAssignable, Category = "GASShooter|GSCharacter")
	FCharacterDiedDelegate OnCharacterDied;

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual void FinishDying();

	virtual void AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags);

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	TArray<FARTDamageNumber> DamageNumberQueue;
	FTimerHandle DamageNumberTimer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASShooter|GSHeroCharacter")
	TSubclassOf<class UGameplayEffect> DeathEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Audio")
	class USoundCue* DeathSound;

	//ABILITY SYSTEM STUFFS
protected:
	UPROPERTY()
	class UARTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UARTCharacterAttributeSet* AttributeSetBase;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UARTGameplayAbility>> CharacterAbilities;

	// These effects are only applied one time on startup, Health regen, stamina regen...etc
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UPROPERTY(EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UARTDamageTextWidgetComponent> DamageNumberClass;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void ShowDamageNumber();

	//FOR AI/Manual Blueprint Ability Activation
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UARTGameplayAbility*>& ActiveAbilities);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);

	//crouch
	void BeginCrouch();
	void EndCrouch();	

	

	//bind ASC input
	bool ASCInputBound = false;
	void BindASCInput();

	virtual void Restart() override;

public:	
	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EARTAbilityInputID AbilityID) const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetAttackPower() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetArmor() const;

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

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attributes")
	float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetEnergyRegen() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetStaminaRegen() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetMoveSpeed() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	float GetRotateRate() const;

	UFUNCTION(BlueprintCallable, Category = "ART|ARTCharacter|Attribute")
	bool IsAlive() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//ONLY USE THIS FOR RE/SPAWNING
	virtual void SetShield(float Shield);
	virtual void SetHealth(float Health);
	virtual void SetStamina(float Stamina);
};
