#include "InventoryService/InventoryScreen.h"

TArray<FString> UInventoryScreen::GetCollectionWhitelist()
{
	TArray<FString> CollectionWhitelist;
	if (!GConfig->GetArray(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("InventoryScreenCollectionWhitelist"), CollectionWhitelist, GGameIni)) {
		CollectionWhitelist = {};
	}
	return CollectionWhitelist;
}
