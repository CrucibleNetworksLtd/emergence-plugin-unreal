// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/LoadContract.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

ULoadContract* ULoadContract::LoadContract(const UObject* WorldContextObject, UEmergenceDeployment* DeployedContract)
{
	ULoadContract* BlueprintNode = NewObject<ULoadContract>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void ULoadContract::Activate()
{
	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField("contractAddress", this->DeployedContract->Address);
	Json->SetStringField("ABI", this->DeployedContract->Contract->ABI);
	
	Json->SetStringField("network", this->DeployedContract->Blockchain->Name.ToString());
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);


	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	UHttpHelperLibrary::ExecuteHttpRequest<ULoadContract>(
		this, 
		&ULoadContract::LoadContract_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "loadContract",
		"POST",
		60.0F,
		Headers,
		OutputString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadContract request started with JSON, calling LoadContract_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *OutputString);
}

void ULoadContract::LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadContract_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
		Singleton->ContractsWithLoadedABIs.AddUnique(DeployedContract->Address);
		UE_LOG(LogEmergenceHttp, Display, TEXT("Added %s to ContractsWithLoadedABIs array"), *DeployedContract->Address);
		OnLoadContractCompleted.Broadcast(HttpResponse->GetContentAsString(), EErrorCode::EmergenceOk);
	}
	else {
		OnLoadContractCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("LoadContract", StatusCode);
	}
}
