// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/ReadMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UReadMethod* UReadMethod::ReadMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, TArray<FString> Content)
{
	UReadMethod* BlueprintNode = NewObject<UReadMethod>();
	BlueprintNode->ContractAddress = ContractAddress;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UReadMethod::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString;
	if (Content.Num() > 0) {
		ContentString.Append("[");
		for (int i = 0; i < Content.Num(); i++) {
			ContentString.Append("\"" + Content[i] + "\"");
			if (i != Content.Num() - 1) {
				ContentString.Append(",");
			}
		}
		ContentString.Append("]");
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UReadMethod>(
		this, 
		&UReadMethod::ReadMethod_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "readMethod?contractAddress=" + ContractAddress + "&methodName=" + MethodName,
		"POST",
		60.0F,
		Headers,
		ContentString);
	UE_LOG(LogTemp, Display, TEXT("ReadMethod request started with JSON, calling ReadMethod_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogTemp, Display, TEXT("%s"), *ContentString);
}

void UReadMethod::ReadMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogTemp, Display, TEXT("ReadMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnReadMethodCompleted.Broadcast(JsonObject.GetObjectField("message")->GetStringField("response"), EErrorCode::EmergenceOk);
		return;
	}
	OnReadMethodCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ReadMethod", StatusCode);
}
