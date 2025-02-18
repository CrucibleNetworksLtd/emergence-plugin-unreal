// Fill out your copyright notice in the Description page of Project Settings.

#include "VirtualsLibrary.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"

void UVirtualsLibrary::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SendStreamRequest();
}

void UVirtualsLibrary::SendStreamRequest()
{
	UE_LOG(LogTemp, Error, TEXT("Starting SendStreamRequest!"));
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	auto Request = UElizaHttpHelperLibrary::ExecuteHttpRequest<UVirtualsLibrary>(
		this,
		&UVirtualsLibrary::GetStream_HttpRequestComplete,
		"https://virtuals-server.vercel.app/api/streamLogger",
		"GET",
		60.0F,
		Headers
	);
	FHttpRequestStreamDelegate StreamDelegate = FHttpRequestStreamDelegate::CreateUObject(this, &UVirtualsLibrary::OnDataReceived);
	Request->SetResponseBodyReceiveStreamDelegate(StreamDelegate);
}

bool UVirtualsLibrary::OnDataReceived(void* Ptr, int64 Length)
{
	UE_LOG(LogTemp, Log, TEXT("Received %d bytes of data"), Length);
	FString Result = FString(UTF8_TO_TCHAR(reinterpret_cast<char*>(Ptr)), Length);
	UE_LOG(LogTemp, Log, TEXT("we think it said: %s"), *Result);
	if (Result.Len() > 0) {
		Result.ReplaceInline(TEXT("data: "), TEXT(""));
		TSharedPtr<FJsonValue> JsonValue;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			auto MessageData = JsonValue->AsObject();
			FString Type = MessageData->GetStringField(TEXT("type"));
			if (Type == "message") {
				FString Content = MessageData->GetStringField(TEXT("content"));
				int32 Timestamp = MessageData->GetIntegerField(TEXT("timestamp"));
				OnVirtualMessageRecieved.Broadcast(Content);
			}
		}
	}
	return true;
}

void UVirtualsLibrary::GetStream_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		UE_LOG(LogTemp, Display, TEXT("GetStream_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("GetStream_HttpRequestComplete failed! Error: %d"), HttpResponse->GetResponseCode());
	}
	SendStreamRequest(); //restart the stream, don't wanna miss a thing
}

void UVirtualsLibrary::SendVirtualsMessage(UObject* WorldContextObject, FString Message)
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	TSharedPtr<FJsonObject> BodyContentJsonObject = MakeShareable(new FJsonObject);
	BodyContentJsonObject->SetStringField(TEXT("message"), Message);
	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(BodyContentJsonObject.ToSharedRef(), Writer);

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UVirtualsLibrary>(
		this,
		&UVirtualsLibrary::SendVirtualsMessage_HttpRequestComplete,
		"https://virtuals-server.vercel.app/api/handleUserMessage",
		"POST",
		60.0F,
		Headers,
		JsonOutput
	);
}

void UVirtualsLibrary::SendVirtualsMessage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		UE_LOG(LogTemp, Display, TEXT("SendVirtualsMessage_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("SendVirtualsMessage_HttpRequestComplete failed!"));
	}
}
