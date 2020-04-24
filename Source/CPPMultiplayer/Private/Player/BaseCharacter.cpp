// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BaseCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Item/SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CPPMultiplayer/CPPMultiplayer.h"
#include "Combat/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Item/Item.h"
#include "Item/InventoryComponent.h"
#include "Player/InteractionComponent.h"
#include "World/Pickup.h"
#include "Player/BasePlayerController.h"

static int32 DebugAimDrawing = 0;
FAutoConsoleVariableRef CVARDebugAimDrawing(TEXT("COOP.DebugAim"),
	DebugAimDrawing,
	TEXT("Draw Debug For Aim"),
	ECVF_Cheat);

// Sets default values
ABaseCharacter::ABaseCharacter(){

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create default subobject canceled, is using Custom Component for BP instead
	/*SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);*/

	SetReplicates(true);
	SetReplicateMovement(true);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	//movement
	BaseTurnRate = 45;

	//aiming
	IsAiming = false;

	//interaction
	InteractionCheckFrequency = 0.;
	InteractionCheckDistance = 1000.;

	//item, weapon
	WeaponAttackSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast< APlayerController>(GetController());
	CharacterMovementComponent = GetCharacterMovement();

	//only spawn weapon on server
	if (Role == ROLE_Authority) {
		//spawn default weapon

		if (HealthComponent) {
			HealthComponent->OnHealthChanged.AddDynamic(this, &ABaseCharacter::OnHealthChanged);
		}
	}

	ChangeWeapon(StarterWeaponClass);

	if (InventoryComponent) {
		InventoryComponent->SetCapacity(20);
		InventoryComponent->SetWeightCapacity(30);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bIsInteractingOnServer = (HasAuthority() && IsInteracting());

	if ((!HasAuthority() || !bIsInteractingOnServer) && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency) {
		PerformInteractionCheck();
	}
}

void ABaseCharacter::Restart(){
	Super::Restart();
	if (ABasePlayerController* PC = Cast<ABasePlayerController>(GetController())) {
		PC->ShowIngameUI();
	}
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

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABaseCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ABaseCharacter::EndInteract);
}

//directional movement system
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

	if (CameraComponent) {
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

//crouching system
void ABaseCharacter::BeginCrouch()
{
	Crouch();
}

void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}	

//initialize components, read component from BP
void ABaseCharacter::InitializeComponents(UCameraComponent* CameraToSet, 
	USpringArmComponent* SpringArmToSet, USHealthComponent* HealthComp, 
	UInventoryComponent* InventoryComp){
	CameraComponent = CameraToSet;
	SpringArmComponent = SpringArmToSet;
	HealthComponent = HealthComp;
	InventoryComponent = InventoryComp;
}

//aiming system
void ABaseCharacter::Aim(){
	if (Role < ROLE_Authority) {
		ServerAim();
	}
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
	if (DebugAimDrawing > 0 && IsLocallyControlled()) {
		DrawDebugSphere(GetWorld(), Intersection, 10, 12, FColor::Red, false, GetWorld()->GetDeltaSeconds(), 0, 1);
		DrawDebugLine(GetWorld(), ActorLocation, Intersection, FColor::Red, false, GetWorld()->GetDeltaSeconds(), 0, 1);
	}
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, Intersection);
	Controller->SetControlRotation(LookAtRotation);
}

void ABaseCharacter::ServerAim_Implementation(){
	Aim();
}

bool ABaseCharacter::ServerAim_Validate(){
	return true;
}

// Interaction system
void ABaseCharacter::PerformInteractionCheck(){

	if (GetController() == nullptr) {
		return;
	}

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLocation;
	FRotator EyesRotation;
	GetController()->GetPlayerViewPoint(EyesLocation, EyesRotation); 

	/*FVector TraceStart = EyesLocation;
	FVector TraceEnd = (EyesRotation.Vector() * InteractionCheckDistance) + TraceStart ;*/

	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = (GetActorRotation().Vector()* InteractionCheckDistance) + TraceStart;

	FHitResult TraceHit; 
	FCollisionQueryParams QueryParams;  
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)) {
		if (TraceHit.GetActor()) {
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass()))) {
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance) {
					FoundNewInteractable(InteractionComponent);
				}
				else if(Distance > InteractionComponent->InteractionDistance && GetInteractable()){
					CouldNotFindInteractable();
				}
				return;
			}
		}
	}

	CouldNotFindInteractable();
}

void ABaseCharacter::CouldNotFindInteractable(){
	if (IsInteracting()) {
		GetWorldTimerManager().ClearTimer(InteractTimerHandler);
	}

	if (UInteractionComponent* Interactable = GetInteractable()) {
		Interactable->EndFocus(this);

		if (InteractionData.bInteractionHeld) {
			EndInteract();
		}
	}
	InteractionData.ViewedInteractionComponent = nullptr;
}

void ABaseCharacter::FoundNewInteractable(UInteractionComponent* Interactable){
	EndInteract(); 
	if (UInteractionComponent* OldInteractable = GetInteractable()) {
		OldInteractable->EndFocus(this);
	}
	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}

void ABaseCharacter::BeginInteract(){
	if (!HasAuthority()) {
		ServerBeginInteract();
	}

	if (HasAuthority()) {
		PerformInteractionCheck();
	}

	InteractionData.bInteractionHeld = true;

	if (UInteractionComponent* Interactable = GetInteractable() ) {
		Interactable->BeginInteract(this);
		if (FMath::IsNearlyZero(Interactable->InteractionTime)) {
			Interact();
		}
		else {
			GetWorldTimerManager().SetTimer(InteractTimerHandler, this, 
				&ABaseCharacter::Interact, Interactable->InteractionTime, false);
		} 
	}
}

void ABaseCharacter::EndInteract(){
	if (!HasAuthority()) {
		ServerEndInteract();
	}

	InteractionData.bInteractionHeld = false;

	GetWorldTimerManager().ClearTimer(InteractTimerHandler);

	if (UInteractionComponent* Interactable = GetInteractable()) {
		Interactable->EndInteract(this);
	}
}

void ABaseCharacter::ServerBeginInteract_Implementation(){
	BeginInteract();
}

bool ABaseCharacter::ServerBeginInteract_Validate(){
	return true;
}

void ABaseCharacter::ServerEndInteract_Implementation(){
	EndInteract();
}

bool ABaseCharacter::ServerEndInteract_Validate(){
	return true;
}

void ABaseCharacter::Interact(){
	GetWorldTimerManager().ClearTimer(InteractTimerHandler);

	if (UInteractionComponent* Interactable = GetInteractable()) {
		Interactable->Interact(this);
	}
}

bool ABaseCharacter::IsInteracting() const{
	return GetWorldTimerManager().IsTimerActive(InteractTimerHandler);
}

float ABaseCharacter::GetRemainingInteractionTime() const{
	return GetWorldTimerManager().GetTimerRemaining(InteractTimerHandler);
}

//item using system
void ABaseCharacter::UseItem(UItem* Item){
	//if is client, run on server
	if (Role < ROLE_Authority && Item) {
		ServerUseItem(Item);
	}

	//if is server, check if the wanted item is in inventory, if not return
	if (HasAuthority()) {
		if (InventoryComponent && !InventoryComponent->FindItem(Item)) {
			return;
		} 
	}
	
	if (Item) {
		Item->OnUse(this);
		Item->Use(this);
	}
}

void ABaseCharacter::ServerUseItem_Implementation(UItem* Item){
	UseItem(Item);
}

bool ABaseCharacter::ServerUseItem_Validate(UItem* Item){
	return true;
}

void ABaseCharacter::DropItem(UItem* Item, int32 Quantity) {
	if (Quantity <= 0) {
		return;
	}
	//if is client, run on server 
	if (Role < ROLE_Authority) {
		ServerDropItem(Item, Quantity);
		return;
	}

	if (HasAuthority()) {
		const int32 ItemQuantity = Item->GetQuantity();
		const int32 DroppedQuantity = InventoryComponent->ConsumeItem(Item, Quantity);

		//spawn pickup
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector SpawnLocation = GetActorLocation();
		//SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FTransform SpawnTransform(GetActorRotation(), SpawnLocation); 
		
		ensure(PickupClass);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, SpawnTransform, SpawnParams);
		Pickup->InitializePickup(Item->GetClass(), DroppedQuantity);

	}
}

void ABaseCharacter::ServerDropItem_Implementation(UItem* Item, int32 Quantity){
	DropItem(Item, Quantity);
}

bool ABaseCharacter::ServerDropItem_Validate(UItem* Item, int32 Quantity){
	return true;
}

void ABaseCharacter::ChangeWeapon(TSubclassOf<ASWeapon> WeaponToChange){
	ServerChangeWeapon(WeaponToChange);
}

void ABaseCharacter::ServerChangeWeapon_Implementation(TSubclassOf<ASWeapon> WeaponToChange){
	if (CurrentWeapon && StarterWeaponClass == WeaponToChange) {
		return;
	}
	else {
		StarterWeaponClass = WeaponToChange;
	}

	if (CurrentWeapon) {
		CurrentWeapon->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponToChange, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttackSocketName);
	}
}

bool ABaseCharacter::ServerChangeWeapon_Validate(TSubclassOf<ASWeapon> WeaponToChange){
	return true;
}

//firing system
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


bool ABaseCharacter::IsLooting() const{
	return LootSource != nullptr;
}

void ABaseCharacter::LootItem(UItem* ItemToGive){
	if (GetLocalRole() < ROLE_Authority) {
		if (InventoryComponent && LootSource && ItemToGive && LootSource->HasItem(ItemToGive->GetClass(), ItemToGive->GetQuantity())) {
			const FItemAddResult AddResult = InventoryComponent->TryAddItem(ItemToGive);

			if (AddResult.ActualAmountGiven > 0) {
				LootSource->ConsumeItem(ItemToGive, AddResult.ActualAmountGiven);
			}
			else {
				if(ABasePlayerController* PC = Cast<ABasePlayerController>(GetController())) {
					PC->ShowNotification(AddResult.ErrorText);
				}
			}
		}
	}
	else {
		ServerLootItem(ItemToGive);
	}
}

void ABaseCharacter::SetLootSource(UInventoryComponent* NewLootSource) {
	if (NewLootSource && NewLootSource->GetOwner()) {
		NewLootSource->GetOwner()->OnDestroyed.AddUniqueDynamic(this, &ABaseCharacter::OnLootSourceOwnerDestroyed);
	}

	if (HasAuthority()) {
		if (NewLootSource) {
			if (ABaseCharacter* Character = Cast<ABaseCharacter>(NewLootSource->GetOwner())) {
				Character->SetLifeSpan(120.f);
			}
		}
		LootSource = NewLootSource;
	}
	else {
		ServerSetLootSource(NewLootSource);
	}
}

void ABaseCharacter::ServerLootItem_Implementation(UItem* ItemToLoot){
	LootItem(ItemToLoot);
}

bool ABaseCharacter::ServerLootItem_Validate(UItem* ItemToLoot){
	return true;
}

void ABaseCharacter::BeginLootingPlayer(ABaseCharacter* Character){
	if (Character) {
		Character->SetLootSource(InventoryComponent);
	}
}

void ABaseCharacter::OnLootSourceOwnerDestroyed(AActor* DestroyedActor){
	if (HasAuthority() && LootSource && DestroyedActor == LootSource->GetOwner()) {
		ServerSetLootSource(nullptr);
	}
}

void ABaseCharacter::OnRep_LootSource() {
	if (ABasePlayerController* PC = Cast<ABasePlayerController>(GetController())) {
		if (PC->IsLocalController()) {
			if (LootSource) {
				PC->ShowLootMenu(LootSource);
			}
			else {
				PC->HideLootMenu();
			}
		}
	}
}

void ABaseCharacter::ServerSetLootSource_Implementation(UInventoryComponent* NewLootSource){
	SetLootSource(NewLootSource);
}

bool ABaseCharacter::ServerSetLootSource_Validate(UInventoryComponent* NewLootSource){
	return true;
}

//on health changed
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
	}
}

//replication
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentWeapon);
	DOREPLIFETIME(ABaseCharacter, bDied);
	DOREPLIFETIME(ABaseCharacter, IsAiming);
	DOREPLIFETIME(ABaseCharacter, LootSource);
}

//unknown
FVector ABaseCharacter::GetPawnViewLocation() const
{
	if (CameraComponent) {
		return CameraComponent->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}



