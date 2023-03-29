// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "EmergenceDeployment.h"
#include "MyK2Node.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCETESTEDITOR_API UMyK2Node : public UK2Node
{
	GENERATED_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AllocateDefaultPins() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void NodeConnectionListChanged() override;
	UEdGraphPin* ContractPin;
	UEdGraphPin* MethodPin;
	static FName ContractTypeToUnrealPinType(FString ContractType);
	static TArray<TPair<FName, FName>> FindMethodReturnTypes(UEmergenceDeployment* Deployment, FString MethodName);
	TArray<TPair<FName, FName>> MethodReturnTypeMap;
};
