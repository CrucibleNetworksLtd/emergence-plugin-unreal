// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnitConverterFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EEtherUnitType : uint8 {
	WEI = 0,
	KWEI = 1,
	MWEI = 2,
	GWEI = 3,
	TWEI = 4,
	PWEI = 5,
	ETHER = 6,
	KETHER = 7,
	METHER = 8,
	GETHER = 9,
	TETHER = 10
};

UCLASS()
class EMERGENCE_API UUnitConverterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Converts EtherUnit To EtherUnit", CompactNodeTitle = "->"))
	static FString Convert(const FString& source, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, const FString& comaSeparator);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Converts Test", CompactNodeTitle = "->"))
	static FString ConvertTest();
private:
	static FString Convert(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, const FString& comaSeparator);
	static FString ConvertToSmallerPow(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, const FString& comaSeparator, int8 resultLength, FString& result);
	static FString ConvertToBiggerPow(const FString& source, const FString& beforeComa, const FString& afterComa, EEtherUnitType sourceUnit, EEtherUnitType targetUnit, const FString& comaSeparator, int8 resultLength, FString& result);
	static uint8 GetPowFactor(EEtherUnitType unit);
	static int8 GetPowFactorDifference(EEtherUnitType sourceUnit, EEtherUnitType targetUnit);
	static FString TrimRightmostZeros(const FString& source);
	static FString TrimLeftmostZeros(const FString& source);
};
