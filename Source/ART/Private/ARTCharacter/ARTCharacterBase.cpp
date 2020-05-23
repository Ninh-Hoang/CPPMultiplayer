// Fill out your copyright notice in the Description page of Project Settings.


#include "ARTCharacter/ARTCharacterBase.h"
#include "ARTCharacter/ARTCharacterMovementComponent.h"
#include <Components/SceneComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Item/InventoryComponent.h"
#include <Components/CapsuleComponent.h>
#include "ART/ART.h"
#include <Kismet/KismetMathLibrary.h>
#include "Ability/ARTAbilitySystemComponent.h"
#include "ARTCharacter/ARTPlayerState.h"
#include <GameFramework/PlayerState.h>
#include "ARTCharacter/ARTCharacterAttributeSet.h"
#include "Ability/ARTGameplayAbility.h"
#include "Player/BasePlayerController.h"
#include <DrawDebugHelpers.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

// Sets default values
AARTCharacterBase::AARTCharacterBase(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UARTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->SetJumpAllowed(false);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	AzimuthComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Azimuth"));
	AzimuthComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(AzimuthComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	WeaponAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon"));
}

UAbilitySystemComponent* AARTCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AARTCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AARTPlayerState* PS = GetPlayerState<AARTPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.

		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		/*AGDPlayerController* PC = Cast<AGDPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		InitializeFloatingStatusBar();*/
		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		//AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetStamina(GetMaxStamina());
	}
}

void AARTCharacterBase::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	bChangedWeaponLocally = false;
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

AWeapon* AARTCharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}


void AARTCharacterBase::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{
	if (NewWeapon == LastWeapon)
	{
		return;
	}
	// Cancel active weapon abilities
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AbilityTagsToCancel = FGameplayTagContainer(WeaponAbilityTag);
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel);
	}
	UnEquipWeapon(LastWeapon);

	if (NewWeapon) {
		if (AbilitySystemComponent)
		{
			// Clear out potential NoWeaponTag
			AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		}

		// Weapons coming from OnRep_CurrentWeapon won't have the owner set
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->EquipWeapon();
		CurrentWeaponTag = CurrentWeapon->WeaponTag;

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
		}

		UAnimMontage* EquipMontage = CurrentWeapon->GetEquipMontage();

		if (EquipMontage && GetMesh())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage);
		}
	}
	else {
		UnEquipCurrentWeapon();
	}
}

void AARTCharacterBase::UnEquipWeapon(AWeapon* WeaponToUnEquip)
{
	WeaponToUnEquip->UnEquip();
}

void AARTCharacterBase::UnEquipCurrentWeapon()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
	}

	UnEquipWeapon(CurrentWeapon);
	CurrentWeapon = nullptr;
}

void AARTCharacterBase::SpawnDefaultInventory()
{
	if (!HasAuthority())
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (!DefaultInventoryWeaponClasses[i])
		{
			// An empty item was added to the Array in blueprint
			continue;
		}

		AWeapon* NewWeapon = GetWorld()->SpawnActorDeferred<AWeapon>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		NewWeapon->SetOwningCharacter(this);

		NewWeapon->FinishSpawning(FTransform::Identity);

		bool bEquipFirstWeapon = i == 0;

		NewWeapon->EquipWeapon();
		NewWeapon->AddAbilities();
	}
}


int32 AARTCharacterBase::GetAbilityLevel(EARTAbilityInputID AbilityID) const
{
	return 1;
}

int32 AARTCharacterBase::GetCharacterLevel() const
{
	return 1;
}

void AARTCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UARTGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		//AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID)));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void AARTCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

void AARTCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->StartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}

// Called when the game starts or when spawned
void AARTCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultInventory();
}

// Called to bind functionality to input
void AARTCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AARTCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AARTCharacterBase::MoveRight);

	//PlayerInputComponent->BindAxis("LookUp", this, &AARTCharacterBase::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &AARTCharacterBase::LookRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AARTCharacterBase::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AARTCharacterBase::EndCrouch);

	//PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AARTCharacterBase::StartAim);
	//PlayerInputComponent->BindAction("Aim", IE_Released, this, &AARTCharacterBase::StopAim);

	BindASCInput();
	//AbilitySystem->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

void AARTCharacterBase::MoveForward(float AxisValue)
{
	if (CameraComponent) {
		FVector ProjectedVector = UKismetMathLibrary::ProjectVectorOnToPlane(CameraComponent->GetForwardVector(), GetActorUpVector());
		FVector ForwardDirection = ProjectedVector.GetSafeNormal();
		AddMovementInput(ForwardDirection, AxisValue);
	}
}

void AARTCharacterBase::MoveRight(float AxisValue)
{
	if (ensure(CameraComponent)) {
		AddMovementInput(CameraComponent->GetRightVector() * AxisValue);
	}
}

void AARTCharacterBase::LookRight(float AxisValue)
{
	if (ensure(AzimuthComponent)) {
		//FRotator SpringArmRotation = SpringArmComponent->GetRelativeRotation();
		//SpringArmRotation = FRotator(SpringArmRotation.Pitch, SpringArmRotation.Roll+ AxisValue, SpringArmRotation.Yaw);
		//AzimuthComponent->AddRelativeRotation(FRotator(0, AxisValue, 0));
		AzimuthComponent->AddLocalRotation(FRotator(0, AxisValue, 0));
	}
}

void AARTCharacterBase::StartAim()
{
	GetWorld()->GetTimerManager().SetTimer(AimTimerHandler,
		this,
		&AARTCharacterBase::LookAtCursor,
		GetWorld()->GetDeltaSeconds(),
		true);
}

void AARTCharacterBase::StopAim()
{
	GetWorld()->GetTimerManager().ClearTimer(AimTimerHandler);
}

void AARTCharacterBase::LookAtCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	FVector MousePosition;
	//PC->GetMousePosition(MousePosition);
	FVector WorldLocation;
	FVector WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FVector ActorLocation = GetActorLocation();
	FVector Intersection = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 1000, ActorLocation, FVector::UpVector);

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Intersection);
	Controller->SetControlRotation(LookAtRotation);
}

void AARTCharacterBase::BeginCrouch()
{
	Crouch();
}

void AARTCharacterBase::EndCrouch()
{
	UnCrouch();
}

float AARTCharacterBase::GetShield() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetShield();
	}

	return 0.0f;
}

float AARTCharacterBase::GetMaxShield() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxShield();
	}

	return 0.0f;
}

float AARTCharacterBase::GetShieldRegen() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetShieldRegen();
	}

	return 0.0f;
}

float AARTCharacterBase::GetHealth() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AARTCharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float AARTCharacterBase::GetHealthRegen() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetHealthRegen();
	}

	return 0.0f;
}

float AARTCharacterBase::GetStamina() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float AARTCharacterBase::GetMaxStamina() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

float AARTCharacterBase::GetStaminaRegen() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetStaminaRegen();
	}

	return 0.0f;
}

float AARTCharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

void AARTCharacterBase::SetShield(float Shield)
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetHealth(Shield);
	}
}

void AARTCharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void AARTCharacterBase::SetStamina(float Stamina)
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetStamina(Stamina);
	}
}

void AARTCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AARTPlayerState* PS = GetPlayerState<AARTPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UARTAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		BindASCInput();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		/*AGDPlayerController* PC = Cast<AGDPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}*/

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		//InitializeFloatingStatusBar();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		//AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetStamina(GetMaxStamina());
	}
}

// Called every frame
void AARTCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARTCharacterBase::BindASCInput()
{
	if (!ASCInputBound &&  AbilitySystemComponent && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EARTAbilityInputID"), static_cast<int32>(EARTAbilityInputID::Confirm), static_cast<int32>(EARTAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}

void AARTCharacterBase::Restart()
{
	Super::Restart();
	if (ABasePlayerController* PC = Cast<ABasePlayerController>(GetController())) {
		PC->ShowIngameUI();
	}
}

bool AARTCharacterBase::IsAlive() const {
	return GetHealth() > 0.0f;
}

/* Called every frame */ 
void Tick(float DeltaTime) {

}

void AARTCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AARTCharacterBase, CurrentWeapon, COND_SimulatedOnly);
}
