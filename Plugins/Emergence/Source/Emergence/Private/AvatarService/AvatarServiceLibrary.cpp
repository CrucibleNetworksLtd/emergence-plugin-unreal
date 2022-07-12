// Fill out your copyright notice in the Description page of Project Settings.


#include "AvatarService/AvatarServiceLibrary.h"
#include "Kismet/KismetStringLibrary.h"

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

FEmergenceAvatarData UAvatarServiceLibrary::FindAvatarFromString(TArray<FEmergenceAvatarResult> Avatars, FString AvatarString)
{
	TArray<FString> AvatarStringParts = UKismetStringLibrary::ParseIntoArray(AvatarString, ":", true);
	for (int i = 0; i < Avatars.Num(); i++) {
		for (int j = 0; j < Avatars[i].Avatars.Num(); j++) {
			if (Avatars[i].chain == AvatarStringParts[0] &&
				Avatars[i].contractAddress == AvatarStringParts[1] &&
				Avatars[i].tokenId == AvatarStringParts[2] &&
				Avatars[i].Avatars[j].GUID == AvatarStringParts[3])
			{
				FEmergenceAvatarData ReturnValue;
				ReturnValue.AvatarNFT = Avatars[i];
				ReturnValue.Avatar = Avatars[i].Avatars[j];
				return ReturnValue;
			}
	}
	}
	return FEmergenceAvatarData();
}
