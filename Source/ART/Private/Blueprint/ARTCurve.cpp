// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/ARTCurve.h"

UARTCurve::UARTCurve()
{
	ARTCurveData.Add(FARTCurveData("Curve_0", FLinearColor::MakeRandomColor()));
}

UARTCurve::~UARTCurve()
{
}

float UARTCurve::GetCurveValue(FGameplayTag QueryTag, float InTime) const
{
	for (const auto& Data : ARTCurveData)
		if (Data.CurveTag == QueryTag)
			return Data.Curve.Eval(InTime);
	return 0;
}

TArray<FRichCurveEditInfoConst> UARTCurve::GetCurves() const
{
	TArray<FRichCurveEditInfoConst> CurveEditInfos;
	for (const auto& Data : ARTCurveData)
		CurveEditInfos.Add(FRichCurveEditInfoConst(&Data.Curve, Data.Name));
	return CurveEditInfos;
}

TArray<FRichCurveEditInfo> UARTCurve::GetCurves()
{
	TArray<FRichCurveEditInfo> CurveEditInfos;
	for (auto& Data : ARTCurveData)
		CurveEditInfos.Add(FRichCurveEditInfo(&Data.Curve, Data.Name));
	return CurveEditInfos;
}

bool UARTCurve::IsValidCurve(FRichCurveEditInfo CurveInfo)
{
	for (const auto& Data : ARTCurveData)
		if (&Data.Curve == CurveInfo.CurveToEdit)
			return true;
	return false;
}

FLinearColor UARTCurve::GetCurveColor(FRichCurveEditInfo CurveInfo) const
{
	for (const auto& Data : ARTCurveData)
		if (CurveInfo.CurveToEdit == &Data.Curve)
			return Data.Color;
	return FLinearColor::White;
}


bool UARTCurve::operator==(const UARTCurve& Curve) const
{
	for (int i = 0; i < ARTCurveData.Num(); i++)
		if (!(ARTCurveData[i].Curve == Curve.ARTCurveData[i].Curve))
			return false;
	return true;
}


#if WITH_EDITOR


void UARTCurve::MakeCurveNameUnique(int CurveIdx)
{
	FARTCurveData& Curve = ARTCurveData[CurveIdx];

	// Find Name Base
	FString BaseName = Curve.Name.ToString();
	int NewNameIdx = 0;
	FString NewName = BaseName;

	int UnderscoreIdx;
	if (BaseName.FindLastChar('_', UnderscoreIdx))
	{
		FString Left = BaseName.Left(UnderscoreIdx);
		FString Right = BaseName.RightChop(UnderscoreIdx + 1);
		if (Right.IsNumeric())
		{
			BaseName = Left;
			NewNameIdx = FCString::Atoi(*Right);
		}
	}

	// Collect Names
	TSet<FName> NameList;
	for (int i = 0; i < ARTCurveData.Num(); i++)
	{
		if (i != CurveIdx)
		{
			NameList.Add(ARTCurveData[i].Name);
		}
	}

	// Find Unique Name
	while (NameList.Contains(FName(*NewName)))
	{
		NewName = FString::Printf(TEXT("%s_%d"), *BaseName, ++NewNameIdx);
	}

	Curve.Name = FName(*NewName);
}

void UARTCurve::PreEditChange(class FEditPropertyChain& PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	OldCurveCount = ARTCurveData.Num();
}

void UARTCurve::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);

	const FName PropName = e.GetPropertyName();
	int CurveIdx = e.GetArrayIndex("ARTCurveData");

	switch (e.ChangeType)
	{
	case EPropertyChangeType::ArrayAdd:
		if (OldCurveCount < ARTCurveData.Num())
		{
			ARTCurveData[CurveIdx].Name = "Curve_0";
			ARTCurveData[CurveIdx].Color = FLinearColor::MakeRandomColor();
			MakeCurveNameUnique(CurveIdx);
		}
		break;

	case EPropertyChangeType::Duplicate:
		// For whatever reason, duplicate adds the new item in the index before the selected
		// but we want to fix up the name on the later one, not the earlier...
		if (0 <= CurveIdx && CurveIdx + 1 < ARTCurveData.Num())
		{
			MakeCurveNameUnique(CurveIdx + 1);
		}
		break;

	case EPropertyChangeType::ValueSet:
		if (0 <= CurveIdx)
		{
			if (PropName == "Name")
			{
				MakeCurveNameUnique(CurveIdx);
			}
			if (PropName == "Color")
			{
				ARTCurveData[CurveIdx].Color.A = 1.0f;
			}
			if (PropName == "CurveTag")
			{
			}
		}
		break;

	case EPropertyChangeType::ArrayClear:
		break;
	}

	OnCurveMapChanged.Broadcast(this);
}

#endif
