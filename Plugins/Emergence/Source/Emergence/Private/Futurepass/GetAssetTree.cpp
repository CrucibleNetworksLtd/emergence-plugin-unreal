// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "Futurepass/GetAssetTree.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"

UGetAssetTree* UGetAssetTree::GetAssetTree(UObject* WorldContextObject, FString TokenId, FString CollectionId)
{
	UGetAssetTree* BlueprintNode = NewObject<UGetAssetTree>();
	BlueprintNode->TokenId = TokenId;
	BlueprintNode->CollectionId = CollectionId;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetAssetTree::Activate()
{
	if (!TokenId.IsEmpty() && !CollectionId.IsEmpty()) {
		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>{ "content-type", "application/json" });
		
		FString Content = R"({"query":"query Asset($tokenId: String!, $collectionId: CollectionId!) {\n  asset(tokenId: $tokenId, collectionId: $collectionId) {\n    assetTree {\n      data\n    }\n  }\n}","variables":{"tokenId":")" + TokenId + R"(","collectionId":")" + CollectionId + R"("}})";
		Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetAssetTree>(
			this,
			&UGetAssetTree::GetAssetTree_HttpRequestComplete,
			"https://6b20qa1273.execute-api.us-west-2.amazonaws.com/graphql",
			"POST",
			60.0F,
			Headers,
			Content);
		UE_LOG(LogEmergenceHttp, Display, TEXT("UGetAssetTree request started, calling UGetAssetTree_HttpRequestComplete on request completed."));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("One or more of UGetAssetTree's inputs were empty."));
		OnGetAssetTreeCompleted.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
	}
}

void UGetAssetTree::GetAssetTree_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetAssetTree_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());

	if (StatusCode == EErrorCode::EmergenceOk) {
		TSharedPtr<FJsonValue> JsonValue;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TSharedPtr<FJsonObject> Object = JsonValue->AsObject();
			if (Object) {
				auto DataArray = Object->GetObjectField("data")->GetObjectField("asset")->GetObjectField("assetTree")->GetObjectField("data")->GetArrayField("@graph");

			}
		}
	}
}