// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Math/Rotator.h"

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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void ABaseCharacter::MoveForward(float AxisValue)
{
	//AddMovementInput(GetActorForwardVector() * AxisValue);
	if (ensure(CameraComponent)) {
		FVector CameraForward = CameraComponent->GetForwardVector();
		FRotator CameraRotation = CameraComponent->GetComponentRotation();
		float SpringArmPitch = SpringArmComponent->GetRelativeRotation().Pitch;
		FRotator Rotation = FRotator(-SpringArmPitch, -CameraRotation.Yaw, - CameraRotation.Roll);
		FVector TrueForwardVector = Rotation.RotateVector(CameraForward);
		AddMovementInput(TrueForwardVector * AxisValue);
	}
}

void ABaseCharacter::MoveRight(float AxisValue)
{
	//AddMovementInput(GetActorRightVector() * AxisValue);
	if (ensure(CameraComponent)) {
		AddMovementInput(CameraComponent->GetRightVector() * AxisValue);
	}
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
}

void ABaseCharacter::InitializeComponents(UCameraComponent* CameraToSet, USpringArmComponent* SpringArmToSet, USceneComponent* AzimuthToSet){
	CameraComponent = CameraToSet;
	SpringArmComponent = SpringArmToSet;
	AzimuthComponent = AzimuthToSet;
}

