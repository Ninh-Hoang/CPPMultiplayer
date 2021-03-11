// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AbilityTask/AT_MontageDeltaCorrection.h"
#include "Ability/ARTAbilitySystemComponent.h"
#include "ART/ART.h"

UAT_MontageDeltaCorrection::UAT_MontageDeltaCorrection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	LocationDeltaCorrectionActivated = false;
	RotationDeltaCorrectionActivated = false;
	UseActorTarget = false;
	LocationCorrectionIndex = 0;
	RotationCorrectionIndex = 0;

	OffSetVector = FVector(0, 0, 0);
	OffSetRotation = FRotator(0, 0, 0);

	RootEndLocation = FVector(0, 0, 0);
	RootEndRotation = FRotator(0, 0, 0);

	CurrentLocationCorrection = FVector(0, 0, 0);;
	CurrentRotationCorrection = FRotator(0, 0, 0);

	DeltaCorrectionTimeLocation = 0.f;
	RemainingDeltaTimeLocation = 0.f;

	DeltaCorrectionTimeRotation = 0.f;
	RemainingDeltaTimeRotation = 0.f;

	LocationAlpha = 0.f;
	RotationAlpha = 0.f;
}


UAT_MontageDeltaCorrection* UAT_MontageDeltaCorrection::MontageDeltaCorrect(
	UGameplayAbility* InOwningAbility,
	FName TaskInstanceName,
	USkeletalMeshComponent* InSkeletal,
	UAnimMontage* InMontageToCorrect,
	FGameplayTagContainer InLocationEventTags,
	FGameplayTagContainer InRotationEventTags,
	FVector InCorrectTarget,
	FRotator InCorrectRotation,
	AActor* InActorTarget,
	float InMontagePlayRate,
	float InRootMotionTranslationScale,
	float InExpoAngle)
{
	UAT_MontageDeltaCorrection* MyObj = NewAbilityTask<UAT_MontageDeltaCorrection>(InOwningAbility, TaskInstanceName);
	MyObj->Skeletal = InSkeletal;
	MyObj->MontageToCorrect = InMontageToCorrect;
	MyObj->LocationEventTags = InLocationEventTags;
	MyObj->RotationEventTags = InRotationEventTags;
	MyObj->CorrectTarget = InCorrectTarget;
	MyObj->CorrectRotation = InCorrectRotation;
	MyObj->ActorTarget = InActorTarget;
	MyObj->MontagePlayRate = InMontagePlayRate;
	MyObj->RootMotionTranslationScale = InRootMotionTranslationScale;
	MyObj->ExpoAngle = InExpoAngle;

	//combine tags
	MyObj->CombineEventTags.AppendTags(MyObj->LocationEventTags);
	MyObj->CombineEventTags.AppendTags(MyObj->RotationEventTags);
	return MyObj;
}

void UAT_MontageDeltaCorrection::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	if (Skeletal == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid Mesh"), *FString(__FUNCTION__));
		return;
	}

	UARTAbilitySystemComponent* ARTAbilitySystemComponent = GetTargetASC();
	if (AbilitySystemComponent)
	{
		// Bind to event callback
		EventHandle = ARTAbilitySystemComponent->AddGameplayEventTagContainerDelegate(
			CombineEventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				this, &UAT_MontageDeltaCorrection::OnGameplayEvent));

		if (IsValid(ActorTarget))
		{
			UseActorTarget = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("UARTAbilityTask_DeltaCorrection called on invalid AbilitySystemComponent"));
	}
	SetWaitingOnAvatar();
}

void UAT_MontageDeltaCorrection::TickTask(float DeltaTime)
{
	if (LocationDeltaCorrectionActivated)
	{
		LocationAlpha += (DeltaTime / RemainingDeltaTimeLocation);
		LocationAlpha = FMath::Clamp(LocationAlpha, 0.0f, 1.0f);
		FVector LerpOffset = FMath::InterpEaseInOut(FVector(0, 0, 0), OffSetVector, LocationAlpha, ExpoAngle);

		FVector DeltaCorrectionTick = LerpOffset - CurrentLocationCorrection;

		GetAvatarActor()->SetActorLocation(GetAvatarActor()->GetActorLocation() + DeltaCorrectionTick);

		CurrentLocationCorrection = LerpOffset;
		if (!UseActorTarget)
		{
			OffSetVector = CorrectTarget - RootEndLocation;
			//DrawDebugSphere(GetWorld(), RootEndLocation, 200.0f,12, FColor::Red, false, 0, 0, 2);
			//DrawDebugSphere(GetWorld(), CorrectTarget, 200.0f,12, FColor::Green, false, 0, 0, 2);
			//DrawDebugSphere(GetWorld(), CorrectTarget, 200.0f, 12, FColor::Cyan, false, 0, 0, 2);
		}
		else
		{
			OffSetVector = ActorTarget->GetActorLocation() - RootEndLocation;
			//DrawDebugSphere(GetWorld(), ActorTarget->GetActorLocation(), 200.0f, 12, FColor::Yellow, false, 0, 0, 2);
		}
	}
	if (RotationDeltaCorrectionActivated)
	{
		RotationAlpha += (DeltaTime / RemainingDeltaTimeRotation);
		RotationAlpha = FMath::Clamp(RotationAlpha, 0.0f, 1.0f);
		FRotator LerpOffset = FMath::InterpEaseInOut(FRotator(0, 0, 0), OffSetRotation, RotationAlpha, ExpoAngle);

		FRotator DeltaCorrectionTick = LerpOffset - CurrentRotationCorrection;

		GetAvatarActor()->SetActorRotation(GetAvatarActor()->GetActorRotation() + DeltaCorrectionTick);

		CurrentRotationCorrection = LerpOffset;
		if (!UseActorTarget)
		{
			//OffSetVector = CorrectTarget - RootEndLocation;
			//DrawDebugSphere(GetWorld(), CorrectTarget, 200.0f, 12, FColor::Cyan, false, 0, 0, 2);
		}
		else
		{
			//OffSetVector = ActorTarget->GetActorLocation() - RootEndLocation;
			//DrawDebugSphere(GetWorld(), ActorTarget->GetActorLocation(), 200.0f, 12, FColor::Yellow, false, 0, 0, 2);
		}
	}
}

void UAT_MontageDeltaCorrection::OnDestroy(bool AbilityEnded)
{
	UARTAbilitySystemComponent* ARTAbilitySystemComponent = GetTargetASC();
	if (ARTAbilitySystemComponent)
	{
		ARTAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(CombineEventTags, EventHandle);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::OnDestroy(AbilityEnded);
}

void UAT_MontageDeltaCorrection::ExternalSetCorrectionLocation(FVector InLocation, AActor* InActor)
{
	if (IsValid(InActor))
	{
		UseActorTarget = true;
		ActorTarget = InActor;
	}
	else
	{
		UseActorTarget = false;
		CorrectTarget = InLocation;
	}
}

void UAT_MontageDeltaCorrection::ExternalSetCorrectionRotation(FRotator InRotator)
{
	CorrectRotation = InRotator;
}

void UAT_MontageDeltaCorrection::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	UAnimInstance* AnimInstance = Skeletal->GetAnimInstance();
	if (AnimInstance)
	{
		if (EventTag.MatchesAny(LocationEventTags))
		{
			/*
			* DELTA CORRECTION FOR LOCATION INIT
			*/
			LocationDeltaCorrectionActivated = true;

			//interpolation
			LocationAlpha = 0.0f;
			CurrentLocationCorrection = FVector(0, 0, 0);

			//set Movement mode
			//Cast<UARTCharacterMovementComponent>(Ability->GetActorInfo().MovementComponent)->SetMovementMode(EMovementMode::MOVE_Flying);

			///get active montage instance
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToCorrect);

			//set time
			DeltaCorrectionTimeLocation = Payload->EventMagnitude / MontagePlayRate;
			RemainingDeltaTimeLocation = DeltaCorrectionTimeLocation;

			//get rootmotion transform
			float IntOut;
			FRootMotionMovementParams RootParms;
			MontageInstance->SimulateAdvance(Payload->EventMagnitude / MontagePlayRate, IntOut, RootParms);
			FTransform RootMotionTransform = RootParms.GetRootMotionTransform();

			FRotator RootTemp = FRotator(0,0,0);
			FVector RootMotionTranslation = RootMotionTransform.GetTranslation() * RootMotionTranslationScale;
			FRotator RootMotionRotation = RootMotionTransform.GetRotation().Rotator();
			Skeletal->TransformFromBoneSpace(Skeletal->GetBoneName(0),
			                                 RootMotionTranslation,
			                                 RootMotionRotation,
			                                 RootEndLocation,
			                                 RootTemp);

			//DrawDebugSphere(GetWorld(), RootEndLocation, 200.0f,12, FColor::Red, false, 3, 0, 2);

			//calculate offset distance and rotation
			//offsetgravity
			if (!UseActorTarget) OffSetVector = CorrectTarget - RootEndLocation;
			else OffSetVector = ActorTarget->GetActorLocation() - RootEndLocation;

			OnLocationCorrectStart.Broadcast(LocationCorrectionIndex);

			FTimerDelegate TimerCallback;
			TimerCallback.BindLambda([this]
			{
				LocationDeltaCorrectionActivated = false;
				OnLocationCorrectEnd.Broadcast(LocationCorrectionIndex);
				LocationCorrectionIndex++;
			});
			GetWorld()->GetTimerManager().SetTimer(LocationDeltaTimerHandle, TimerCallback, DeltaCorrectionTimeLocation,
			                                       false);
		}
		else
		{
			/*
			* DELTA CORRECTION FOR ROTATION INIT
			*/
			RotationDeltaCorrectionActivated = true;

			//interpolation
			RotationAlpha = 0.0f;
			CurrentRotationCorrection = FRotator(0, 0, 0);

			//set Movement mode
			//Cast<UARTCharacterMovementComponent>(Ability->GetActorInfo().MovementComponent)->SetMovementMode(EMovementMode::MOVE_Flying);

			///get active montage instance
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToCorrect);

			//set time
			DeltaCorrectionTimeRotation = Payload->EventMagnitude / MontagePlayRate;
			RemainingDeltaTimeRotation = DeltaCorrectionTimeRotation;

			//get rootmotion transform
			float IntOut;
			FRootMotionMovementParams RootParms;
			MontageInstance->SimulateAdvance(DeltaCorrectionTimeRotation, IntOut, RootParms);
			FTransform RootMotionTransform = RootParms.GetRootMotionTransform();

			FVector RootTemp = FVector(0,0,0);
			FVector RootMotionTranslation = RootMotionTransform.GetTranslation() * RootMotionTranslationScale;
			FRotator RootMotionRotation = RootMotionTransform.GetRotation().Rotator();
			Skeletal->TransformFromBoneSpace(Skeletal->GetBoneName(0),
			                                 RootMotionTranslation,
			                                 RootMotionRotation,
			                                 RootTemp,
			                                 RootEndRotation);

			//calculate offset distance and rotation
			//offsetgravity
			RootEndRotation = GetAvatarActor()->GetActorRotation();
			OffSetRotation = CorrectRotation - RootEndRotation;

			FVector StartTrace = GetAvatarActor()->GetActorLocation();
			FVector EndTrace = StartTrace + GetAvatarActor()->GetActorForwardVector() * 300;
			FVector EndTraceCorrect = EndTrace;
			EndTraceCorrect = CorrectRotation.RotateVector(EndTraceCorrect);
			/*DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 3.f, 0.f, 3.f);
			if (!UseActorTarget)
				DrawDebugLine(GetWorld(), StartTrace, EndTraceCorrect, FColor::Green, false, 3.f, 0.f, 3.f);
			else
				DrawDebugSphere(GetWorld(), ActorTarget->GetActorLocation(), 200.0f, 12, FColor::Yellow, false,
				                0, 0, 2);*/

			OnRotationCorrectStart.Broadcast(RotationCorrectionIndex);

			FTimerDelegate TimerCallback;
			TimerCallback.BindLambda([this]
			{
				RotationDeltaCorrectionActivated = false;
				OnRotationCorrectEnd.Broadcast(RotationCorrectionIndex);
				RotationCorrectionIndex++;
			});
			GetWorld()->GetTimerManager().SetTimer(RotationDeltaTimerHandle, TimerCallback, DeltaCorrectionTimeRotation,
			                                       false);
		}
	}
}

UARTAbilitySystemComponent* UAT_MontageDeltaCorrection::GetTargetASC()
{
	return Cast<UARTAbilitySystemComponent>(AbilitySystemComponent);
}
