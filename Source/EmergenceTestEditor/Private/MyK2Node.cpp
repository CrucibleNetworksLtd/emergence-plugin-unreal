// Fill out your copyright notice in the Description page of Project Settings.


#include "MyK2Node.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraphNode.h"
#include "EmergenceDeployment.h"
#include "EdGraphSchema_K2.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonTypes.h"

FText UMyK2Node::GetNodeTitle(ENodeTitleType::Type TitleType) const {
	return FText::FromString("Test");
}

void UMyK2Node::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UMyK2Node::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	
	UEdGraphNode::FCreatePinParams PinParams;
	PinParams.bIsReference = false;
	ContractPin = CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_Object, UEmergenceDeployment::StaticClass(), "Deployment", PinParams);
	ContractPin->bNotConnectable = true;
	MethodPin = CreatePin(EEdGraphPinDirection::EGPD_Input, UEdGraphSchema_K2::PC_Struct, FEmergenceContractMethod::StaticStruct(), "Method Name");
	MethodPin->bNotConnectable = true;

	for (int i = 0; i < MethodReturnTypeMap.Num(); i++) {
		FName OutputName = MethodReturnTypeMap[i].Key;
		FName OutputType = MethodReturnTypeMap[i].Value;
		UE_LOG(LogTemp, Display, TEXT("Attempting to create pin '%s' of type '%s'"), *OutputName.ToString(), *OutputType.ToString());
		CreatePin(EEdGraphPinDirection::EGPD_Output, OutputType, OutputName);
	}
}

void UMyK2Node::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	UE_LOG(LogTemp, Display, TEXT("UMyK2Node::NotifyPinConnectionListChanged: %s"), *Pin->GetDisplayName().ToString());
}

void UMyK2Node::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
	
	//On Method Pin changed
	if(Pin == MethodPin){
		if (UEmergenceDeployment* Deployment = Cast<UEmergenceDeployment>(ContractPin->DefaultObject)) {
			FString MethodString = MethodPin->GetDefaultAsString().Replace(TEXT("(MethodName=\""), TEXT("")).Replace(TEXT("\")"), TEXT(""));
			MethodReturnTypeMap = FindMethodReturnTypes(Deployment, MethodString);
			this->ReconstructNode();
		}
		else {
			UE_LOG(LogTemp, Display, TEXT("Failed to cast ContractPin->DefaultObject to Deployment"));
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("UMyK2Node::PinDefaultValueChanged: %s, %s, %s"), *Pin->GetDisplayName().ToString(), *Pin->PinType.PinCategory.ToString(), *Pin->PinType.PinSubCategoryObject.Get()->GetName());
}

void UMyK2Node::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
	UE_LOG(LogTemp, Display, TEXT("UMyK2Node::NodeConnectionListChanged"));
}

FName UMyK2Node::ContractTypeToUnrealPinType(FString ContractType)
{
	if (ContractType == "bool") {
		return UEdGraphSchema_K2::PC_Boolean;
	}
	if (ContractType == "bytes4") {
		return UEdGraphSchema_K2::PC_Byte;
	}
	if (ContractType == "uint256") {
		return UEdGraphSchema_K2::PC_Int;
	}
	if (ContractType == "uint128") {
		return UEdGraphSchema_K2::PC_Int;
	}
	if (ContractType == "int256") {
		return UEdGraphSchema_K2::PC_Int;
	}
	if (ContractType == "string") {
		return UEdGraphSchema_K2::PC_String;
	}
	return FName();
}

TArray<TPair<FName, FName>> UMyK2Node::FindMethodReturnTypes(UEmergenceDeployment* Deployment, FString MethodName)
{
	TArray<TPair<FName, FName>> PinTypes;
	if (!Deployment->Contract->ABI.IsEmpty()) {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Deployment->Contract->ABI);
		TSharedPtr<FJsonValue> JsonObject;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
			auto ABIArray = JsonObject->AsArray();
			for (int k = 0; k < ABIArray.Num(); k++) {
				TSharedPtr<FJsonObject> InterfaceObject = ABIArray[k].Get()->AsObject();
				//if its a function and the name matches the supplied name
				if (InterfaceObject->GetStringField("type") == "function" && InterfaceObject->GetStringField("name") == MethodName) {
					TArray<TSharedPtr<FJsonValue>> Outputs = InterfaceObject->GetArrayField("outputs");
					for (int i = 0; i < Outputs.Num(); i++) {
						FString Type = Outputs[i]->AsObject()->GetStringField("type");
						if (Type == "tuple") {
							TArray<TSharedPtr<FJsonValue>> Components = Outputs[i]->AsObject()->GetArrayField("components");
							for (int j = 0; j < Components.Num(); j++) {
								FName ArgumentName = FName(FString("Argument") + FString::FromInt(i) + FString(", ") + FString::FromInt(j));
								FString ComponentsType = Components[j]->AsObject()->GetStringField("type");
								PinTypes.Add(TPair<FName, FName>(ArgumentName, ContractTypeToUnrealPinType(ComponentsType)));
							}
						}
						else {
							FName ArgumentName = FName(FString("Argument") + FString::FromInt(i));
							PinTypes.Add(TPair<FName, FName>(ArgumentName, ContractTypeToUnrealPinType(Type)));
						}
					}
				}
			}
		}
	}
	return PinTypes;
}
