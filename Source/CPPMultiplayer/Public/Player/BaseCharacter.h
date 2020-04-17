// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class ASWeapon;
class USHealthComponent;
class UItem;
class UInventoryComponent;
class UInteractionComponent;
class APickup;

USTRUCT()
struct FInteractionData {
	GENERATED_BODY()

	FInteractionData() {
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.;
		bInteractionHeld = false;
	}

	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent;

	UPROPERTY()
	float LastInteractionCheckTime;

	UPROPERTY()
	bool bInteractionHeld; 

};

UCLASS()
class CPPMULTIPLAYER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

//class components and initialization
public:
	// Sets default values for this character's properties
	ABaseCharacter();

	//setup play input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//player owned components
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* AzimuthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	//initialize, read player components from BP
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeComponents(UCameraComponent* CameraToSet,
	USpringArmComponent* SpringArmToSet, USHealthComponent* HealthComp, UInventoryComponent* InventoryComp);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//call every tick
	virtual void Tick(float DeltaTime) override;

	virtual void Restart() override;

//movement control
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);

	//crouch
	void BeginCrouch();
	void EndCrouch();

	//firing
	void StartFire();
	void StopFire();
	
//aiming
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAim();

protected:
	UPROPERTY(Replicated)
	bool IsAiming;
	
	FTimerHandle AimTimerHandler;

	UFUNCTION()
	void LookAtCursor();
	void Aim();


//health system
protected:
	//pawn die previously
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType, 
		class AController* InstigatedBy, AActor* DamageCauser);

	

//interaction system functions
public:
	bool IsInteracting() const;

	float GetRemainingInteractionTime() const;

protected:
	FTimerHandle InteractTimerHandler;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;

	UPROPERTY()
	FInteractionData InteractionData;
	void PerformInteractionCheck();

	void CouldNotFindInteractable();
	void FoundNewInteractable(UInteractionComponent* Interactable);

	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	FORCEINLINE UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

//item using 
public:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<APickup> PickupClass;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseItem(UItem* Item);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(UItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem(UItem* Item, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropItem(UItem* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void ChangeWeapon(TSubclassOf<ASWeapon> WeaponToChange);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeWeapon(TSubclassOf<ASWeapon> WeaponToChange);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ASWeapon* CurrentWeapon;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttackSocketName;

//looting
public:
	UFUNCTION(BlueprintPure, Category = "Looting")
	bool IsLooting() const;

	UFUNCTION(BlueprintCallable, Category = "Looting")
	void LootItem(UItem* ItemToGive);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLootItem(UItem* ItemToLoot);

	UFUNCTION(BlueprintCallable)
	void SetLootSource(UInventoryComponent* NewLootSource);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_LootSource, BlueprintReadOnly)
	UInventoryComponent* LootSource;

	UFUNCTION()
	void BeginLootingPlayer(ABaseCharacter* Character);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetLootSource(UInventoryComponent* NewLootSource);

	UFUNCTION()
	void OnLootSourceOwnerDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void OnRep_LootSource();

	

//wtf is this one for?
protected:
	virtual FVector GetPawnViewLocation() const override;

};
