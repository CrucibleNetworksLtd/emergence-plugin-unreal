// Copyright Crucible Networks Ltd 2024. All Rights Reserved.


#include "HotjoinSessionLibrary.h"
#include "EmergenceSingleton.h"
#include "WebLogin/CustodialLogin.h"

bool UHotjoinSessionLibrary::HotjoinSessionFromData(const UObject* ContextObject, FString EncodedSessionData)
{
	if (!ContextObject) {
		UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: no context object."));
		return false; //error no context object
	}

	auto Singleton = UEmergenceSingleton::GetEmergenceManager(ContextObject);

	if (!Singleton) {
		UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: couldn't get singleton."));
		return false; //error couldn't get singleton
	}

	FEmergenceCustodialLoginOutput Claims;
	if (UCustodialLogin::GetClaimsFromFutureverseAuthToken(Claims, EncodedSessionData)) {
		Singleton->UseAccessToken = false;
		Singleton->CompleteLoginViaWebLoginFlow(Claims, EErrorCode::EmergenceOk);
		return true;
	}
	else {
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientInvalidResponse);
	}

	return false;
}

void UHotjoinSessionLibrary::HotjoinSessionFromAppLaunchArgs(const UObject* ContextObject)
{
	FString CommandLine = FCommandLine::Get();
	TArray<FString> Args;
	CommandLine.ParseIntoArray(Args, TEXT(" "), false);
	if (Args.Num() >= 2) {
		FString DecodedToken;
		if (FBase64::Decode(Args[1], DecodedToken)) {
			UHotjoinSessionLibrary::HotjoinSessionFromData(ContextObject, Args[1]);
		}
		else {
			UE_LOG(LogEmergence, Error, TEXT("Failed to decode provided base64'd FV token."))
		}
	}
}
