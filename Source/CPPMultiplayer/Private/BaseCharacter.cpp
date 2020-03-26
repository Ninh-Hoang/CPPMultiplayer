// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CPPMultiplayer/CPPMultiplayer.h"
#include "SHealthComponent.h"

static int32 DebugAimDrawing = 0;
FAutoConsoleVariableRef CVARDebugAimDrawing(TEXT("COOP.DebugAim"),
	DebugAimDrawing,
	TEXT("Draw Debug For Aim"),
	ECVF_Cheat);

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create default subobject canceled, is using Custom Component for BP instead
	/*SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);*/

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	BaseTurnRate = 45;
	IsAiming = false;
	WeaponAttackSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast< APlayerController>(GetController());
	CharacterMovementComponent = GetCharacterMovement();

	//spawn default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttackSocketName);
	}

	if (HealthComponent) {
		HealthComponent->OnHealthChanged.AddDynamic(this, &ABaseCharacter::OnHealthChanged);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ABaseCharacter::LookRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed,this, &ABaseCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released,this, &ABaseCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jump);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::Aim);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::StopFire);
}

void ABaseCharacter::MoveForward(float AxisValue)
{
	//AddMovementInput(GetActorForwardVector() * AxisValue);
	/*if (ensure(CameraComponent)) {
		FVector CameraForward = CameraComponent->GetForwardVector();
		FRotator CameraRotation = CameraComponent->GetComponentRotation();
		float SpringArmPitch = SpringArmComponent->GetRelativeRotation().Pitch;
		FRotator Rotation = FRotator(-SpringArmPitch, -CameraRotation.Yaw, - CameraRotation.Roll);
		FVector TrueForwardVector = Rotation.RotateVector(CameraForward);
		AddMovementInput(CameraComponent->GetForwardVector() * AxisValue);
	}*/
	/*if ((Controller != NULL) && (AxisValue != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}*/

	if (ensure(CameraComponent)) {
		FVector ProjectedVector = UKismetMathLibrary::ProjectVectorOnToPlane(CameraComponent->GetForwardVector(), GetActorUpVector());
		FVector ForwardDirection = ProjectedVector.GetSafeNormal();
		AddMovementInput(ForwardDirection, AxisValue);
	}
}

void ABaseCharacter::MoveRight(float AxisValue)
{
	//AddMovementInput(GetActorRightVector() * AxisValue);
	if (ensure(CameraComponent)) {
		AddMovementInput(CameraComponent->GetRightVector() * AxisValue);
	}
	/*if ((Controller != NULL) && (AxisValue != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, AxisValue);
	}*/
}


void ABaseCharacter::BeginCrouch()
{
	Crouch();
}

void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}	

void ABaseCharacter::LookRight(float AxisValue){
	/*if (ensure(AzimuthComponent)) {
		FRotator CurrentRotation = AzimuthComponent->GetRelativeRotation();
		FRotator AddRotation = FRotator(0, AxisValue, 0);
		AzimuthComponent->AddLocalRotation(AddRotation);
		//UE_LOG(LogTemp, Warning, TEXT("%f"), AxisValue);
	}*/
	//AddControllerYawInput(AxisValue * BaseTurnRate* GetWorld()->GetDeltaSeconds());

	if (ensure(SpringArmComponent)) {
		//FRotator SpringArmRotation = SpringArmComponent->GetRelativeRotation();
		//SpringArmRotation = FRotator(SpringArmRotation.Pitch, SpringArmRotation.Roll+ AxisValue, SpringArmRotation.Yaw);
		SpringArmComponent->AddRelativeRotation(FRotator(0, 0, AxisValue));
	}
}

void ABaseCharacter::InitializeComponents(UCameraComponent* CameraToSet, USpringArmComponent* SpringArmToSet, USHealthComponent* HealthComp){
	CameraComponent = CameraToSet;
	SpringArmComponent = SpringArmToSet;
	HealthComponent = HealthComp;
}

FVector ABaseCharacter::GetPawnViewLocation() const
{
	if (CameraComponent) {
		return CameraComponent->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ABaseCharacter::Aim(){
	if (IsAiming) {
		IsAiming = !IsAiming;
		CharacterMovementComponent->bOrientRotationToMovement = true;
		CharacterMovementComponent->bUseControllerDesiredRotation = false;
		GetWorld()->GetTimerManager().ClearTimer(AimTimerHandler);

	}
	else {
		CharacterMovementComponent->bOrientRotationToMovement = false;
		CharacterMovementComponent->bUseControllerDesiredRotation = true;

		GetWorld()->GetTimerManager().SetTimer(AimTimerHandler, 
			this,
			&ABaseCharacter::LookAtCursor, 
			GetWorld()->GetDeltaSeconds(),
			true);
		IsAiming = !IsAiming;
	}
}

void ABaseCharacter::LookAtCursor() {
	APlayerController* PC = Cast<APlayerController>(GetController());
	FVector MousePosition;
	//PC->GetMousePosition(MousePosition);
	FVector WorldLocation;
	FVector WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FVector ActorLocation = GetActorLocation();
	FVector Intersection = FMath::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 1000, ActorLocation, FVector::UpVector);
	if (DebugAimDrawing > 0) {
		DrawDebugSphere(GetWorld(), Intersection, 10, 12, FColor::Red, false, GetWorld()->GetDeltaSeconds(), 0, 1);
		DrawDebugLine(GetWorld(), ActorLocation, Intersection, FColor::Red, false, GetWorld()->GetDeltaSeconds(), 0, 1);
	}
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, Intersection);
	Controller->SetControlRotation(LookAtRotation);
}

void ABaseCharacter::OnHealthChanged(USHealthComponent* HealthComp, 
	float Health, float HealthDelta,
	const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bDied) {
		//ded
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//DetachFromControllerPendingDestroy();

		SetLifeSpan(5.0);
	}
}

void ABaseCharacter::StartFire(){
	if (CurrentWeapon) {
		CurrentWeapon->StartFire();
	}
}

void ABaseCharacter::StopFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StopFire();
	}
}



