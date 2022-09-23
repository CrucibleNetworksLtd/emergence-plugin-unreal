// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceChainObject.h"
#include "EmergenceContract.h"
#include "EmergenceDeployment.generated.h"

UCLASS(BlueprintType, CollapseCategories, meta=(DisplayName="Emergence Deployed Contract"))
class EMERGENCE_API UEmergenceDeployment : public UObject
{

  GENERATED_BODY()

public:
	UEmergenceDeployment() {};

	UEmergenceDeployment(FString Address, UEmergenceChain* Blockchain, UEmergenceContract* Contract);

	UFUNCTION(BlueprintPure)
	static UEmergenceDeployment* CreateEmergenceDeployment(FString Address, UEmergenceChain* Blockchain, UEmergenceContract* Contract);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEmergenceChain* Blockchain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEmergenceContract* Contract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Address;
  
};