// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/EmergenceScreen.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "InventoryScreen.generated.h"

UENUM(BlueprintType)
enum class EEmergenceInventoryBlockchain : uint8 {
    Any = 0,
    Ethereum = 1,
    Polygon = 2,
    Flow = 3,
    Tezos = 4,
    Solana = 5,
    Immutablex = 6
};

USTRUCT(BlueprintType)
struct FEmergenceInventoryFilterSetCategories
{

    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool Avatars = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool Props = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool Clothing = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool Weapons = true;
};

USTRUCT(BlueprintType)
struct FEmergenceInventoryFilterSet
{

    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool UseSearch = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "UseSearch"))
        FString Search;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool UseCategories = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowInnerProperties, EditCondition = "UseCategories"))
        FEmergenceInventoryFilterSetCategories Categories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool UseBlockchain = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "UseBlockchain"))
        EEmergenceInventoryBlockchain Blockchain;

};

UCLASS()
class UInventoryScreen : public UEmergenceScreen
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEmergenceInventoryFilterSet Filters;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, FEmergenceInventoryItem, Item);

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnItemSelected OnItemSelected;
};
