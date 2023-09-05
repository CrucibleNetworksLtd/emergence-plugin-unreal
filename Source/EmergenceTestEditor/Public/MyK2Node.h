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
	UEdGraphPin* DataPin;
	virtual bool IsNodePure() const override { return true; }
	static FName ContractTypeToUnrealPinType(FString ContractType);
	static const UFunction* UnrealPinTypeToConversionFunction(FName UnrealPinType);
	static TArray<TPair<FName, FName>> FindMethodReturnTypes(UEmergenceDeployment* Deployment, FString MethodName);
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	TArray<TPair<FName, FName>> MethodReturnTypeMap;
};
