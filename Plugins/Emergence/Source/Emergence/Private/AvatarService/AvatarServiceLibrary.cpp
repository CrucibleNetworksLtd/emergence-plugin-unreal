// Fill out your copyright notice in the Description page of Project Settings.


#include "AvatarService/AvatarServiceLibrary.h"


bool UAvatarServiceLibrary::GetEmergencePreferredNodeURL(FString Blockchain, FString &URL) {
	const TMap<FString, FString> BlockchainToURL = {
		{"RINKEBY", "https://rinkeby.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"},
		{"POLYGON","https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"},
		{"Ethereum","https://mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"}
	};
	if (BlockchainToURL.Contains(Blockchain)) {
		URL = *BlockchainToURL.Find(Blockchain);
		return true;
	}
	else {
		return false;
	}
}