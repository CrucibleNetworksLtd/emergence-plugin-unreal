// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "InventoryService/InventoryByOwner.h"
#include "InventoryByOwnerTest.generated.h"


UCLASS()
class EMERGENCETESTS_API AInventoryByOwnerTest : public AFunctionalTest
{
	GENERATED_BODY()
	
	virtual void StartTest() override {
		auto InventoryByOwner = UInventoryByOwner::InventoryByOwner(this, "0x238678df4F2CeeCC8b09C2b49eb94458682e6A4C");
		InventoryByOwner->OnInventoryByOwnerCompleted.AddDynamic(this, &AInventoryByOwnerTest::InventoryByOwnerCompleted);
		InventoryByOwner->Activate();
	};
	
	UFUNCTION()
	void InventoryByOwnerCompleted(FEmergenceInventory Inventory, EErrorCode StatusCode){
		if (Inventory.items.Num() > 0) {
			for (FEmergenceInventoryItem Item : Inventory.items) {
				if(Item.contract == "0x074534dF6174759a5Ae3ad81e3BCdfc0F940f6a6" &&
				Item.blockchain == "POLYGON"){
					FinishTest(EFunctionalTestResult::Succeeded, "Found the NFT");
					return;
				}
			}
		}
		FinishTest(EFunctionalTestResult::Failed, "Could not find the known NFT");
	}
};
