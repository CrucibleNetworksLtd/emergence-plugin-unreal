// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/InventoryItemWidget.h"

void UInventoryItemWidget::GetTextureFromURL(FString URL) {
	GetTextureFromUrlRequest = UGetTextureFromUrl::TextureFromUrl(URL, this, true);
	GetTextureFromUrlRequest->OnGetTextureFromUrlCompleted.AddDynamic(this, &UInventoryItemWidget::OnGetTextureFromURLCompleted);
	GetTextureFromUrlRequest->Activate();
}

void UInventoryItemWidget::StopConnections()
{
	if (GetTextureFromUrlRequest) {
		GetTextureFromUrlRequest->OnGetTextureFromUrlCompleted.Clear();
		if (GetTextureFromUrlRequest->GetDataRequest) {
			if (GetTextureFromUrlRequest->GetDataRequest->GetStatus() == EHttpRequestStatus::Processing) {
				UE_LOG(LogEmergenceHttp, Warning, TEXT("Cancelled GetTextureFromURL data request within InventoryItemWidget."))
					GetTextureFromUrlRequest->GetDataRequest->OnProcessRequestComplete().Unbind();
				GetTextureFromUrlRequest->GetDataRequest->CancelRequest();
			}
		}
		if (GetTextureFromUrlRequest->ConvertGifRequest) {
			if (GetTextureFromUrlRequest->ConvertGifRequest->GetStatus() == EHttpRequestStatus::Processing) {
				UE_LOG(LogEmergenceHttp, Warning, TEXT("Cancelled GetTextureFromURL gif conversion request within InventoryItemWidget."))
					GetTextureFromUrlRequest->ConvertGifRequest->OnProcessRequestComplete().Unbind();
				GetTextureFromUrlRequest->ConvertGifRequest->CancelRequest();
			}
		}
	}
}
