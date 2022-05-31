// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UHttpHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	inline static FString APIBase;

	inline static const FString DatabaseAPIPublic = TEXT("https://pfy3t4mqjb.execute-api.us-east-1.amazonaws.com/staging/");
	inline static const FString DatabaseAPIPrivate = TEXT("https://57l0bi6g53.execute-api.us-east-1.amazonaws.com/staging/");
	inline static const FString InventoryService = TEXT("https://dysaw5zhak.us-east-1.awsapprunner.com/InventoryService/");
	inline static const FString AvatarService = TEXT("https://dysaw5zhak.us-east-1.awsapprunner.com/AvatarSystem/");

	template<typename T>
	inline static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ExecuteHttpRequest(T* FunctionBindObject, void(T::* FunctionBindFunction)(FHttpRequestPtr, FHttpResponsePtr, bool), const FString& URL, const FString& Verb = TEXT("GET"), const float& Timeout = 60.0F, const TArray<TPair<FString, FString>>& Headers = TArray<TPair<FString, FString>>(), const FString& Content = FString())
	{
		static_assert(std::is_base_of<UObject, T>::value, "T not derived from UObject");
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		if (FunctionBindFunction && FunctionBindObject) {
			HttpRequest->OnProcessRequestComplete().BindUObject(FunctionBindObject, FunctionBindFunction);
		}
		
		FString FinalURL;

		//switch IPFS to our public node...
		if (URL.Contains(TEXT("ipfs://"))) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("ExecuteHttpRequest found IPFS, replacing with public node..."));

			FString IPFSNode = TEXT("https://ipfs.io/ipfs/");
			FString CustomIPFSNode = "";
			if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("IPFSNode"), CustomIPFSNode, GGameIni))
			{
				if (CustomIPFSNode != "") {
					UE_LOG(LogEmergenceHttp, Display, TEXT("Found custom IPFS node in game config, replacing with \"%s\""), *CustomIPFSNode);
					IPFSNode = CustomIPFSNode;
				}
			}

			FinalURL = URL.Replace(TEXT("ipfs://"), *IPFSNode);
		}
		else {
			FinalURL = URL;
		}

		HttpRequest->SetURL(FinalURL);
		HttpRequest->SetVerb(Verb);
		HttpRequest->SetTimeout(Timeout);
		
		//Handle headers and logging of the headers
		FString HeaderLogText;
		if (Headers.Num() > 0) {
			HeaderLogText = "\nHeaders:\n";
			for (int i = 0; i < Headers.Num(); i++) {
				HttpRequest->SetHeader(Headers[i].Key, Headers[i].Value);
				HeaderLogText.Append(Headers[i].Key + ": " + Headers[i].Value + "\n");
			}
		}

		if (Content.Len() > 0 && HttpRequest->GetHeader("Content-Type").Len() > 0) {
			HttpRequest->SetContentAsString(Content);
		}

		UE_LOG(LogEmergenceHttp, Display, TEXT("Sent %s request to \"%s\", timing out in %f %s \n%s"), *Verb, *FinalURL, Timeout, *HeaderLogText, *Content);
		HttpRequest->ProcessRequest();
		return HttpRequest;
	};
};
