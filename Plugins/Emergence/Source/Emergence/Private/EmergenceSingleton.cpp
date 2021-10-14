
#include "EmergenceSingleton.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "GameDelegates.h"

//for HTTP services
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UEmergenceSingleton::UEmergenceSingleton() {
}

TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> UEmergenceSingleton::GlobalManagers{};


UEmergenceSingleton* UEmergenceSingleton::GetEmergenceManager(const UObject* ContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (GameInstance)
	{
		TWeakObjectPtr<UEmergenceSingleton>& Manager = GlobalManagers.FindOrAdd(GameInstance);
		if (!Manager.IsValid())
		{
			Manager = NewObject<UEmergenceSingleton>(GameInstance);
			Manager->SetGameInstance(GameInstance);
			Manager->Init();
		}
		UE_LOG(LogTemp, Warning, TEXT("Text %s"), *Manager->GetFName().ToString());
		return Manager.Get();
	}
	UE_LOG(LogTemp, Warning, TEXT("Text %s"), "No manager avalible, whats going on?");
	return nullptr;
}

UEmergenceSingleton* UEmergenceSingleton::ForceInitialize(const UObject* ContextObject)
{
	return GetEmergenceManager(ContextObject);
}

void UEmergenceSingleton::Init()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().AddUObject(this, &UEmergenceSingleton::Shutdown);
	AddToRoot();
}

void UEmergenceSingleton::Shutdown()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().RemoveAll(this);

	RemoveFromRoot();
	MarkPendingKill();
}

//HTTP Services
void UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			UE_LOG(LogTemp, Display, TEXT("EnumerateFiles request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnGetWalletConnectURIRequestCompleted.Broadcast(*ResponseStr, true);
			return;
		}
	}
	OnGetWalletConnectURIRequestCompleted.Broadcast(FString(), false);
}

void UEmergenceSingleton::GetWalletConnectURI()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete);
	HttpRequest->SetURL("http://localhost:50733/api/getwalletconnecturi");
	HttpRequest->SetHeader(TEXT("accept"), TEXT("text/plain"));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}


