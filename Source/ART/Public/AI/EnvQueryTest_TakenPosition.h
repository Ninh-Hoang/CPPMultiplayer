// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_TakenPosition.generated.h"

/**
 * 
 */

UENUM()
namespace EEnvTestDistance
{
	enum Type
	{
		Distance3D,
        Distance2D,
        DistanceZ,
        DistanceAbsoluteZ UMETA(DisplayName = "Distance Z (Absolute)")
    };
}

UCLASS()
class ART_API UEnvQueryTest_TakenPosition : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	/** testing mode */
    UPROPERTY(EditDefaultsOnly, Category=Distance)
	TEnumAsByte<EEnvTestDistance::Type> TestMode;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
