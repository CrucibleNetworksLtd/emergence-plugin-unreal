// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HotjoinSessionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UHotjoinSessionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//Given a session data string, join the session. Returns true if everything went as expected
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "ContextObject"))
	static bool HotjoinSessionFromData(const UObject* ContextObject, FString EncodedSessionData);

	//Reads the application launch arguments to find the FV token to hotjoin to
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "ContextObject"))
	static void HotjoinSessionFromAppLaunchArgs(const UObject* ContextObject);
};
