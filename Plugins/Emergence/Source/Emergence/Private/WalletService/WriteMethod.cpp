// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/WriteMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UWriteMethod* UWriteMethod::WriteMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, TArray<FString> Content)
{
	UWriteMethod* BlueprintNode = NewObject<UWriteMethod>();
	BlueprintNode->ContractAddress = ContractAddress;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UWriteMethod::Activate()
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

	UHttpHelperLibrary::ExecuteHttpRequest<UWriteMethod>(
		this, 
		&UWriteMethod::WriteMethod_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "writeMethod?contractAddress=" + ContractAddress + "&methodName=" + MethodName,
		"POST",
		60.0F,
		Headers,
		ContentString);
	UE_LOG(LogTemp, Display, TEXT("WriteMethod request started with JSON, calling WriteMethod_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogTemp, Display, TEXT("%s"), *ContentString);
}

void UWriteMethod::WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogTemp, Display, TEXT("WriteMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnWriteMethodCompleted.Broadcast(JsonObject.GetObjectField("message")->GetStringField("response"), EErrorCode::EmergenceOk);
		return;
	}
	OnWriteMethodCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("WriteMethod", StatusCode);
}
