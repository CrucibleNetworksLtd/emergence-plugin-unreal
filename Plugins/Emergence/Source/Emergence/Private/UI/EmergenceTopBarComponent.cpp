// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "UI/EmergenceTopBarComponent.h"
#include "WalletService/GetBalance.h"
#include "EmergenceSingleton.h"
#include "UnitConverterFunctionLibrary.h"
#include "WalletService/ReadMethod.h"
#include "WalletService/ContractInterfaces/ERC20Contract.h"

void UEmergenceTopBarComponent::StartGetBalanceTextAsync() {

	CurrencyDisplayText = ""; //Clear the currency display text

	//We need both of these no matter if we're using an ERC20 or not
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(this);
	this->CurrencyDisplayChainData = UEmergenceChain::GetEmergenceChainDataFromConfig(this);

	//figure out if we want to use an ERC20 as the balance
	bool UseERC20AsBalance;
	if (!GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("UseERC20AsBalance"), UseERC20AsBalance, GGameIni)) {
		UseERC20AsBalance = false; //if we failed to get the config, default to false
	}

	FString ERC20Address;
	if (UseERC20AsBalance) {
		if (!GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ERC20Address"), ERC20Address, GGameIni)) {
			ERC20Address = "";
		}
	}

	if (!UseERC20AsBalance || ERC20Address.IsEmpty()) { //if we aren't using an ERC20 as the balance, just do a GetBalance request
		UGetBalance* GetBalanceRequest = UGetBalance::GetBalance(Singleton->GetCachedAddress(), CurrencyDisplayChainData);
		GetBalanceRequest->OnGetBalanceCompleted.AddDynamic(this, &UEmergenceTopBarComponent::GetBalanceResponseHandler);
		GetBalanceRequest->Activate();
	}
	else { //if we are using an ERC20 as the balance
		//Create deployed contract for this
		UEmergenceDeployment* ERC20Deployment = NewObject<UEmergenceDeployment>();
		ERC20Deployment->Blockchain = this->CurrencyDisplayChainData;
		ERC20Deployment->Address = ERC20Address;
		ERC20Deployment->Contract = NewObject<UERC20Contract>();
		UReadMethod* BalanceOfRequest =  UReadMethod::ReadMethod(this, ERC20Deployment, FEmergenceContractMethod("balanceOf"), { Singleton->GetCachedAddress() });
		BalanceOfRequest->OnReadMethodCompleted.AddDynamic(this, &UEmergenceTopBarComponent::BalanceOfResponseHandler);
		BalanceOfRequest->Activate();
		UReadMethod* SymbolRequest = UReadMethod::ReadMethod(this, ERC20Deployment, FEmergenceContractMethod("symbol"), {});
		SymbolRequest->OnReadMethodCompleted.AddDynamic(this, &UEmergenceTopBarComponent::SymbolResponseHandler);
		SymbolRequest->Activate();
	}
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
		UE_LOG(LogTemp, Display, TEXT("GetBalanceResponseHandler came back with %s"), *Balance);
		if (FCString::Atoi(*Balance) != 0) {
			this->ReturnedBalance = Balance;
			CurrencyDisplayText = GetBalanceText();
			BalanceTextUpdated();
		}
		else {
			CurrencyDisplayText = "COULDNT CONV";
		}
	}
	else {
		CurrencyDisplayText = "ERM ERROR";
	}
}

void UEmergenceTopBarComponent::BalanceOfResponseHandler(FString Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		TSharedPtr<FJsonObject> JsonInternalObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		FJsonSerializer::Deserialize(Reader, JsonInternalObject);
		UE_LOG(LogTemp, Display, TEXT("BalanceOfResponseHandler came back with %s"), *JsonInternalObject->GetStringField("balance"));
		this->ReturnedBalance = JsonInternalObject->GetStringField("balance");
		CurrencyDisplayText = GetBalanceText();
		BalanceTextUpdated();
	}
	else {
		CurrencyDisplayText = "ERM ERROR";
	}
}

void UEmergenceTopBarComponent::SymbolResponseHandler(FString Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->ReturnedSymbol = Response;
		BalanceTextUpdated();
	}
	else {
		this->ReturnedSymbol = "SYM ERROR";
	}
}

FString UEmergenceTopBarComponent::GetBalanceText()
{
	if (this->ReturnedSymbol.IsEmpty()) {
		return "";
	}

	if (this->ReturnedBalance == "0") {
		return "0 " + this->ReturnedSymbol;
	}
	FString IntergerSide, DecimalSide;
	FString AsEther = UUnitConverterFunctionLibrary::Convert(this->ReturnedBalance, EEtherUnitType::WEI, EEtherUnitType::ETHER, ".");
	if (AsEther.Contains(".")) {
		AsEther.Split(".", &IntergerSide, &DecimalSide);
		return IntergerSide + "." + DecimalSide.Left(3) + " " + this->ReturnedSymbol;
	}
	else {
		return AsEther + " " + this->ReturnedSymbol;
	}
}
