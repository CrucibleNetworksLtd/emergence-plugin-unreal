// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmergenceCarousel.generated.h"

class UEmergenceCarouselItem;
/**
 * 
 */
UCLASS()
class EMERGENCE_API UEmergenceCarousel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(TArray<UEmergenceCarouselItem*>& itemsParam);

private:
	TArray<UEmergenceCarouselItem*> items;
};
