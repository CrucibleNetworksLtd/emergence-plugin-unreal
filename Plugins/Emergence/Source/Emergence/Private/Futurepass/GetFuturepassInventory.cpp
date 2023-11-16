// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Futurepass/GetFuturepassInventory.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetFuturepassInventory* UGetFuturepassInventory::GetFuturepassInventory(UObject* WorldContextObject, TArray<FString> Addresses){
	UGetFuturepassInventory* BlueprintNode = NewObject<UGetFuturepassInventory>();
	BlueprintNode->Addresses = Addresses;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetFuturepassInventory::Activate() {
	FString AddressString;
	for (int i = 0; i < Addresses.Num(); i++) {
		if (i != 0) {
			AddressString = AddressString + ",";
		}
		AddressString = AddressString + "\"" + Addresses[i] + "\"";
	}

	FString Content = R"({"query":"query ExampleQuery($addresses: [Address]!, $chainLocations: [BlockchainLocationInput], $first: Int) {\n  nfts(addresses: $addresses, chainLocations: $chainLocations, first: $first) {\n    edges {\n      node {\n        assetType\n        collection {\n          chainId\n          chainType\n          location\n          name\n        }\n        tokenIdNumber\n        metadata {\n          properties\n        }\n      }\n    }\n  }\n}\n","variables":{"addresses":[)" + AddressString + R"(],"first":100},"operationName":"ExampleQuery"})";
	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetFuturepassInventory>(
		this,
		nullptr,
		"https://adx1wewtnh.execute-api.us-west-2.amazonaws.com/api/graphql",
		"POST",
		60.0F, //give the user lots of time to mess around setting high gas fees
		{TPair<FString, FString>("Content-Type","application/json")},
		Content, false);
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		EErrorCode StatusCode;
		FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetFuturepassInventory_HttpRequestComplete: %s"), *res->GetContentAsString());

		if (StatusCode == EErrorCode::EmergenceOk) {
			TSharedPtr<FJsonValue> JsonValue;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(res->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
				TSharedPtr<FJsonObject> Object = JsonValue->AsObject();
				if (Object) {
					auto NFTArray = Object->GetObjectField("data")->GetObjectField("nfts")->GetArrayField("edges");

					TArray<FEmergenceInventoryItem> Items;

					for (auto NFTNode : NFTArray) {
						auto NFTData = NFTNode->AsObject()->GetObjectField("node");

						FEmergenceInventoryItem Item;
						Item.contract = NFTData->GetObjectField("collection")->GetStringField("location");
						Item.tokenId = "#" + FString::FromInt(NFTData->GetNumberField("tokenIdNumber"));
						Item.blockchain = NFTData->GetObjectField("collection")->GetStringField("chainId") + ":" + NFTData->GetObjectField("collection")->GetStringField("chainType");
						if (NFTData->GetObjectField("metadata")->GetObjectField("properties")->HasTypedField<EJson::String>("name")) {
							Item.meta.name = NFTData->GetObjectField("metadata")->GetObjectField("properties")->GetStringField("name");
						}
						else {
							Item.meta.name = Item.tokenId;
						}
						UE_LOG(LogTemp, Display, TEXT("%s"), *Item.meta.name);
						NFTData->GetObjectField("metadata")->GetObjectField("properties")->TryGetStringField("description", Item.meta.description);
						FEmergenceInventoryItemsMetaContent Content;
						Content.mimeType = "image/png";
						Content.url = NFTData->GetObjectField("metadata")->GetObjectField("properties")->GetStringField("image");
						Item.meta.content.Add(Content);
						Items.Add(Item);
					}

					FEmergenceInventory Inventory = FEmergenceInventory(Items.Num(), Items);
					this->OnGetFuturepassInventoryCompleted.Broadcast(Inventory, StatusCode);
				}
			}
		}
		else {
			this->OnGetFuturepassInventoryCompleted.Broadcast(FEmergenceInventory(), StatusCode);
			return;
		}
	});
	Request->ProcessRequest();
}