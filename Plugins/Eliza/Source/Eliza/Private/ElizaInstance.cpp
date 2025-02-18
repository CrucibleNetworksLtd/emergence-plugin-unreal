#include "ElizaInstance.h"

UElizaInstance* UElizaInstance::CreateElizaInstance(FString LocationURL, EElizaAPIType APIType, FString FleekAPIKey, FString FleekAgentId) {
	UElizaInstance* ReturnObject = NewObject<UElizaInstance>();
	ReturnObject->ElizaInstance.APIType = APIType;
	ReturnObject->ElizaInstance.LocationURL = LocationURL;
	ReturnObject->ElizaInstance.FleekAPIKey = FleekAPIKey;
	ReturnObject->ElizaInstance.FleekAgentId = FleekAgentId;
	return ReturnObject;
}