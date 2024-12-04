// Copyright Crucible Networks Ltd 2024. All Rights Reserved.


#include "HotjoinSessionLibrary.h"
#include "EmergenceSingleton.h"

FString UHotjoinSessionLibrary::PrepareSessionForHotjoin(const UObject* ContextObject)
{
	if (!ContextObject) {
		UE_LOG(LogEmergence, Error, TEXT("PrepareSessionForHotjoin Error: no context object."));
		return FString(); //error no context object
	}

	auto Singleton = UEmergenceSingleton::GetEmergenceManager(ContextObject);

	if (!Singleton) {
		UE_LOG(LogEmergence, Error, TEXT("PrepareSessionForHotjoin Error: couldn't get singleton."));
		return FString(); //error couldn't get singleton
	}

	TSharedPtr<FJsonObject> SessionDataJson = MakeShared<FJsonObject>();
	
	FString Address = Singleton->GetCachedAddress(true);
	if (Address != FString("-1")) {
		SessionDataJson->SetStringField("ChecksummedAddress", Address);
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("PrepareSessionForHotjoin Error: You can't prepare a session for hotjoin if you don't have a session!"));
		return FString(); //No session, terminate now!
	}

	if (!Singleton->DeviceID.IsEmpty()) { //if we have a device id, 
		SessionDataJson->SetStringField("DeviceId", Singleton->DeviceID); //the user has logged in with WalletConnect
	}
	//if we have an address but don't have a device id, we must be using web login flow
	//however, if we don't have the bool set, something terrible has happened and we shouldn't try to pass along this session
	else if (!Singleton->UsingWebLoginFlow) { 
		UE_LOG(LogEmergence, Error, TEXT("PrepareSessionForHotjoin Error: Emergence in weird state. If you see this error, contact Crucible."));
		return FString(); //No session, terminate now!
	}

	//set the access token field if we have one in the current session
	FString AccessToken = Singleton->GetCurrentAccessToken();
	if (AccessToken != FString("-1")) {
		SessionDataJson->SetStringField("EmergenceAccessToken", AccessToken);
	}

	//turn into a json string
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(SessionDataJson.ToSharedRef(), Writer);

	Singleton->PreventEVMServerSessionKilling = true; //don't let the singleton kill the session, as it needs to exist after this program exits

	return FBase64::Encode(OutputString);
}

bool UHotjoinSessionLibrary::HotjoinSessionFromData(const UObject* ContextObject, FString EncodedSessionData)
{
	FString DecodedSessionData;
	if (FBase64::Decode(EncodedSessionData, DecodedSessionData)) {
		if (!ContextObject) {
			UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: no context object."));
			return false; //error no context object
		}

		auto Singleton = UEmergenceSingleton::GetEmergenceManager(ContextObject);

		if (!Singleton) {
			UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: couldn't get singleton."));
			return false; //error couldn't get singleton
		}

		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(DecodedSessionData);
		if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed)) //if we're NOT able to deserialize the JSON sent to the callback
		{
			UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: Couldn't decode JSON."));
			return false; //error couldn't decode json
		}

		if (Singleton->GetCachedAddress(true) != FString("-1")) { //if it doesn't equal the "empty" address
			UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: Can't hot join a session if one already exists."));
			//I worry if we let people try to join a session if they already have one, its could very easily mess up a load of things
			//especially if they have requests in-flight, etc
			return false; //error don't try to hot join a session if you already have / have had a session
		}

		FString ChecksummedAddress;
		if (JsonParsed->TryGetStringField("ChecksummedAddress", ChecksummedAddress)) { //this should always be sent
			Singleton->CurrentChecksummedAddress = ChecksummedAddress;
			Singleton->CurrentAddress = ChecksummedAddress.ToLower();
		}
		else { //if they didn't send this, it isn't valid data
			UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: invalid data."));
			return false; //error no address in data
		}

		
		FString DeviceId;
		if (JsonParsed->TryGetStringField("DeviceId", DeviceId)) { //if we have a device id, this isn't a web login flow session
			Singleton->UsingWebLoginFlow = false;
			Singleton->DeviceID = DeviceId;
		}
		else { //if we don't have a device id, this is a web login flow session implicitly 
			Singleton->UsingWebLoginFlow = true;
		}

		FString EmergenceAccessToken;
		if (JsonParsed->TryGetStringField("EmergenceAccessToken", EmergenceAccessToken)) {
			Singleton->CurrentAccessToken = EmergenceAccessToken;
			Singleton->UseAccessToken = true;
		}
		else {
			Singleton->UseAccessToken = false;
		}
		return true;

	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("HotjoinSessionFromData Error: base64 invalid."));
		return false;
	}
}
