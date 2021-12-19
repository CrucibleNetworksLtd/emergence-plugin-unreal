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
	inline static const FString APIBase = TEXT("http://localhost:50733/api/");
	inline static const FString DatabaseAPIPublic = TEXT("https://pfy3t4mqjb.execute-api.us-east-1.amazonaws.com/staging/");
	inline static const FString DatabaseAPIPrivate = TEXT("https://57l0bi6g53.execute-api.us-east-1.amazonaws.com/staging/");

	template<typename T>
	inline static bool ExecuteHttpRequest(T* FunctionBindObject, void(T::* FunctionBindFunction)(FHttpRequestPtr, FHttpResponsePtr, bool), const FString& URL, const FString& Verb = TEXT("GET"), const float& Timeout = 60.0F, const TArray<TPair<FString, FString>>& Headers = TArray<TPair<FString, FString>>(), const FString& Content = FString())
	{
		static_assert(std::is_base_of<UObject, T>::value, "T not derived from UObject");
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		if (FunctionBindFunction && FunctionBindObject) {
			HttpRequest->OnProcessRequestComplete().BindUObject(FunctionBindObject, FunctionBindFunction);
		}
		HttpRequest->SetURL(URL);
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

		UE_LOG(LogTemp, Display, TEXT("Sent %s request to %s, timing out in %f %s \n%s"), *Verb, *URL, Timeout, *HeaderLogText, *Content);
		return HttpRequest->ProcessRequest();
	};
};
