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
#include "ARTCharacter/ARTPlayerController.h"
#include <DrawDebugHelpers.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include <Ability/ARTAbilitySystemGlobals.h>
#include <Kismet/GameplayStatics.h>
#include "Widget/ARTDamageTextWidgetComponent.h"
#include <Ability/ARTGameplayAbility.h>

// Sets default values
AARTCharacterBase::AARTCharacterBase(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UARTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->SetJumpAllowed(false);

	AzimuthComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Azimuth"));
	AzimuthComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(AzimuthComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag("Effect.RemoveOnDeath");

	SetGenericTeamId(FGenericTeamId(TeamNumber));
}

FGenericTeamId AARTCharacterBase::GetGenericTeamId() const
{
	return FGenericTeamId(TeamNumber);
}

ETeamAttitude::Type AARTCharacterBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const AARTCharacterBase* OtherPawn = Cast<AARTCharacterBase>(&Other)) {
		//Create an alliance with Team with ID 10 and set all the other teams as Hostiles:
		FGenericTeamId OtherTeamID = OtherPawn->GetGenericTeamId();
		if (OtherTeamID == FGenericTeamId(TeamNumber)) {
			return ETeamAttitude::Friendly;
		}
		else {
			return ETeamAttitude::Hostile;
		}
	}
	return ETeamAttitude::Neutral;
}

UAbilitySystemComponent* AARTCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AARTCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		UE_LOG(LogTemp, Warning, TEXT("Remove"));
	}
	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

void AARTCharacterBase::Die()
{
	// Only runs on Server
	if (!HasAuthority()) {
		return;
	}

	RemoveCharacterAbilities();

	if (IsValid(AbilitySystemComponent) && DeathEffect)
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(DeathEffect->GetDefaultObject()), 1.0f, AbilitySystemComponent->MakeEffectContext());

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	OnCharacterDied.Broadcast(this);

	//TODO replace with a locally executed GameplayCue
	if (DeathSound)
	{
		//UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AARTCharacterBase::FinishDying()
{
	Destroy();
}

void AARTCharacterBase::AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags)
{
	DamageNumberQueue.Add(FARTDamageNumber(Damage, DamageNumberTags));

	if (!GetWorldTimerManager().IsTimerActive(DamageNumberTimer))
	{
		GetWorldTimerManager().SetTimer(DamageNumberTimer, this, &AARTCharacterBase::ShowDamageNumber, 0.1, true, 0.0f);
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

void AARTCharacterBase::ShowDamageNumber()
{
	if (DamageNumberQueue.Num() > 0 && IsValid(this))
	{
		UARTDamageTextWidgetComponent* DamageText = NewObject<UARTDamageTextWidgetComponent>(this, DamageNumberClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->SetDamageText(DamageNumberQueue[0].DamageAmount, DamageNumberQueue[0].Tags);

		if (DamageNumberQueue.Num() < 1)
		{
			GetWorldTimerManager().ClearTimer(DamageNumberTimer);
		}

		DamageNumberQueue.RemoveAt(0);
	}
}

//Mostly for AI
bool AARTCharacterBase::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation /*= true*/)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void AARTCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UARTGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

// Called when the game starts or when spawned
void AARTCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	/*SpawnDefaultInventory();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerSyncCurrentWeapon();
	}*/
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

	BindASCInput();
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

float AARTCharacterBase::GetAttackPower() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetAttackPower();
	}

	return 0.0f;
}

float AARTCharacterBase::GetArmor() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetArmor();
	}

	return 0.0f;
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

float AARTCharacterBase::GetEnergy() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetEnergy();
	}

	return 0.0f;
}

float AARTCharacterBase::GetMaxEnergy() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxEnergy();
	}

	return 0.0f;
}

float AARTCharacterBase::GetEnergyRegen() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetEnergyRegen();
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

float AARTCharacterBase::GetRotateRate() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetRotateRate();
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
	if (AARTPlayerController* PC = Cast<AARTPlayerController>(GetController())) {
		//PC->ShowIngameUI();
	}
}

bool AARTCharacterBase::IsAlive() const {
	return GetHealth() > 0.0f;
}

/* Called every frame */ 
void Tick(float DeltaTime) {

}


