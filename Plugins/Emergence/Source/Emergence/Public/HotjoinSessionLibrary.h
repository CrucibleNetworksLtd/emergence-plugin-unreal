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
	//Gets the data about the current session
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "ContextObject"))
	static FString PrepareSessionForHotjoin(const UObject* ContextObject);

	//Given a session data string, join the session. Returns true if everything went as expected
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "ContextObject"))
	static bool HotjoinSessionFromData(const UObject* ContextObject, FString EncodedSessionData);
};
