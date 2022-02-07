// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "UI/EmergenceCarousel.h"
#include "UI/EmergenceCarouselItem.h"

void UEmergenceCarousel::Init(TArray<UEmergenceCarouselItem*>& itemsParam)
{
	items = itemsParam;
}