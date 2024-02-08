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
			UHttpHelperLibrary::GetFutureverseAssetRegistryAPIURL(),
			"POST",
			60.0F,
			Headers,
			Content);
		UE_LOG(LogEmergenceHttp, Display, TEXT("UGetAssetTree request started, calling UGetAssetTree_HttpRequestComplete on request completed."));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("One or more of UGetAssetTree's inputs were empty."));
		OnGetAssetTreeCompleted.Broadcast(TArray<FEmergenceFutureverseAssetTreePart>(), EErrorCode::EmergenceClientFailed);
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
				TArray<FEmergenceFutureverseAssetTreePart> AssetTree;

				for (auto Data : DataArray) {
					FEmergenceFutureverseAssetTreePart AssetTreePartStruct;
					AssetTreePartStruct.Id = Data->AsObject()->GetStringField("@id"); //get the ID of this
					AssetTreePartStruct.RDFType = Data->AsObject()->GetObjectField("rdf:type")->GetStringField("@id"); //get the rdf:type
					
					TArray<FString> PredicateKeys;
					if (Data->AsObject()->Values.GetKeys(PredicateKeys) > 0) {
						for (FString PredicateKey : PredicateKeys) {

							if (PredicateKey == "@id" || PredicateKey == "rdf:type") { //ignore these ones
								continue;
							}

							FEmergenceFutureversePredicateData PredicateStruct;
							TSharedPtr<FJsonObject> Predicate = Data->AsObject()->GetObjectField(PredicateKey);
							PredicateStruct.Id = Predicate->GetStringField("@id");

							TArray<FString> PredicateContentKeys;
							if (Predicate->Values.GetKeys(PredicateContentKeys) > 0) {
								for (FString PredicateContentKey : PredicateContentKeys) {
									if (PredicateContentKey == "@id") { //ignore
										continue;
									}

									FString JsonAsString;
									TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonAsString);
									FJsonSerializer::Serialize(*Predicate->Values.Find(PredicateContentKey), PredicateContentKey, Writer);

									PredicateStruct.AdditonalData.Add(PredicateContentKey, JsonAsString);
								}
							}
							
							AssetTreePartStruct.Predicates.Add(PredicateKey, PredicateStruct);
							
						}
					}
					AssetTree.Add(AssetTreePartStruct);
				}
				OnGetAssetTreeCompleted.Broadcast(AssetTree, EErrorCode::EmergenceClientFailed);
				return;
			}
		}
	}
	OnGetAssetTreeCompleted.Broadcast(TArray<FEmergenceFutureverseAssetTreePart>(), EErrorCode::EmergenceClientFailed);
	UE_LOG(LogEmergenceHttp, Error, TEXT("One or more errors occured trying to parse asset tree."));
	return;
}