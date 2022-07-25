// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AvatarService/AvatarByOwner.h"
#include "AvatarServiceLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceAvatarData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergenceAvatarResult AvatarNFT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergenceAvatarMetadata Avatar;
};

UCLASS()
class EMERGENCE_API UAvatarServiceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Avatar Service")
	static bool GetEmergencePreferredNodeURL(FString Blockchain, FString& URL);

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Avatar Service")
	static FEmergenceAvatarData FindAvatarFromString(TArray<FEmergenceAvatarResult> Avatars, FString AvatarString);
};
