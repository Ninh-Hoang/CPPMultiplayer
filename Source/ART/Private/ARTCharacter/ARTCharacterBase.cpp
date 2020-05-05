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

		//AddStartupEffects();

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

// Called when the game starts or when spawned
void AARTCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AARTCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AARTCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AARTCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AARTCharacterBase::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &AARTCharacterBase::LookRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AARTCharacterBase::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AARTCharacterBase::EndCrouch);

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

void AARTCharacterBase::BeginCrouch()
{
	Crouch();
}

void AARTCharacterBase::EndCrouch()
{
	UnCrouch();
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

float AARTCharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
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



