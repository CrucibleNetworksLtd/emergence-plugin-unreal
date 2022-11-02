// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "UI/EmergenceTopBarComponent.h"
#include "WalletService/GetBalance.h"
#include "EmergenceSingleton.h"
#include "UnitConverterFunctionLibrary.h"

void UEmergenceTopBarComponent::StartGetBalanceTextAsync() {
	CurrencyDisplayText = "";
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(this);
	this->CurrencyDisplayChainData = UEmergenceChain::GetEmergenceChainDataFromConfig(this);
	UGetBalance* GetBalanceRequest = UGetBalance::GetBalance(Singleton->GetCachedAddress(), CurrencyDisplayChainData);
	GetBalanceRequest->OnGetBalanceCompleted.AddDynamic(this, &UEmergenceTopBarComponent::GetBalanceResponseHandler);
	GetBalanceRequest->Activate();
}

bool UEmergenceTopBarComponent::ShouldDisplayBalanceText()
{
	bool ShouldShowBalance;
	if (!GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShowBalance"), ShouldShowBalance, GGameIni)) {
		ShouldShowBalance = true; //if we failed to get the config, default to true
	}
	return ShouldShowBalance;
}

void UEmergenceTopBarComponent::GetBalanceResponseHandler(FString Balance, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		if (FCString::Atoi(*Balance) != 0) {
			FString IntergerSide, DecimalSide;
			UUnitConverterFunctionLibrary::Convert(Balance, EEtherUnitType::WEI, EEtherUnitType::ETHER, ".").Split(".", &IntergerSide, &DecimalSide);
			CurrencyDisplayText = IntergerSide + "." + DecimalSide.Left(3) + " " + CurrencyDisplayChainData->Symbol;
		}
		else {
			CurrencyDisplayText = "0 " + CurrencyDisplayChainData->Symbol;
		}
	}
	else {
		CurrencyDisplayText = "ERROR";
	}
	
}
