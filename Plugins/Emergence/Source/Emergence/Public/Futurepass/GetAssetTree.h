// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "GetAssetTree.generated.h"


USTRUCT(BlueprintType)
struct FFutureverseAssetTreeObject
{

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futurepass|Predicate Data")
	FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futurepass|Predicate Data")
	TMap<FString, FString> AdditonalData;

};

USTRUCT(BlueprintType)
struct FFutureverseAssetTreePath
{

    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futurepass|Asset Tree Part")
    FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futurepass|Asset Tree Part")
	FString RDFType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futurepass|Asset Tree Part")
	TMap<FString, FFutureverseAssetTreeObject> Objects;

};

UCLASS()
class EMERGENCE_API UGetFutureverseAssetTree : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futurepass Methods")
	static UGetFutureverseAssetTree* GetFutureverseAssetTree(UObject* WorldContextObject, FString TokenId, FString CollectionId);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetAssetTreeCompleted, const TArray<FFutureverseAssetTreePath>&, AssetTree, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetAssetTreeCompleted OnGetAssetTreeCompleted;
private:
	void GetAssetTree_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString TokenId, CollectionId;
};
