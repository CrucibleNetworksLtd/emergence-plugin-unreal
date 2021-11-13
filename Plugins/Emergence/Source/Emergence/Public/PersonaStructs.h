#pragma once
#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "PersonaStructs.generated.h"
USTRUCT(BlueprintType)
struct FEmergenceGame
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

	FEmergenceGame() {};

	FEmergenceGame( FString _id, FString _name ){

		id = _id;
		name = _name;
	
	}
	
	FEmergenceGame(FString _json_){
		FEmergenceGame _tmpEmergenceGame;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceGame>(
		_json_,
		&_tmpEmergenceGame,
		0, 0);
		
		id = _tmpEmergenceGame.id;
		name = _tmpEmergenceGame.name;
	}
	
};

USTRUCT(BlueprintType)
struct FEmergencePersonaSettings
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool showStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool receiveContactRequest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool availableOnSearch;

	FEmergencePersonaSettings() {};

	FEmergencePersonaSettings( bool _showStatus, bool _receiveContactRequest, bool _availableOnSearch ){

		showStatus = _showStatus;
		receiveContactRequest = _receiveContactRequest;
		availableOnSearch = _availableOnSearch;
	
	}
	
	FEmergencePersonaSettings(FString _json_){
		FEmergencePersonaSettings _tmpEmergencePersonaSetting;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersonaSettings>(
		_json_,
		&_tmpEmergencePersonaSetting,
		0, 0);
		
		showStatus = _tmpEmergencePersonaSetting.showStatus;
		receiveContactRequest = _tmpEmergencePersonaSetting.receiveContactRequest;
		availableOnSearch = _tmpEmergencePersonaSetting.availableOnSearch;
	}
	
};

USTRUCT(BlueprintType)
struct FEmergenceAvatar
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergenceGame game;

	FEmergenceAvatar() {};

	FEmergenceAvatar( FString _id, FString _url, FEmergenceGame _game ){

		id = _id;
		url = _url;
		game = _game;
	
	}
	
	FEmergenceAvatar(FString _json_){
		FEmergenceAvatar _tmpEmergencePersonaAvatar;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceAvatar>(
		_json_,
		&_tmpEmergencePersonaAvatar,
		0, 0);
		
		id = _tmpEmergencePersonaAvatar.id;
		url = _tmpEmergencePersonaAvatar.url;
		game = _tmpEmergencePersonaAvatar.game;
	}
	
};

USTRUCT(BlueprintType)
struct FEmergencePersona
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString bio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergencePersonaSettings settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergenceAvatar gameAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool deleted{ false };
	
	FEmergencePersona() {};

	FEmergencePersona( FString _id, FString _name, FString _bio, FEmergencePersonaSettings _settings, FEmergenceAvatar _gameAvatar, bool _deleted){
		id = _id;
		name = _name;
		bio = _bio;
		settings = _settings;
		gameAvatar = _gameAvatar;
		deleted = _deleted;
	}
	
	FEmergencePersona(FString _json_){
		FEmergencePersona _tmpEmergencePersona;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersona>(
		_json_,
		&_tmpEmergencePersona,
		0, 0);
		
		id = _tmpEmergencePersona.id;
		name = _tmpEmergencePersona.name;
		bio = _tmpEmergencePersona.bio;
		settings = _tmpEmergencePersona.settings;
		gameAvatar = _tmpEmergencePersona.gameAvatar;
		deleted = _tmpEmergencePersona.deleted;
	}

};

USTRUCT(BlueprintType)
struct FEmergencePersonaListResponse
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEmergencePersona> personas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString selected;

	FEmergencePersonaListResponse() {};

	FEmergencePersonaListResponse( TArray<FEmergencePersona> _personas, FString _selected ){

		personas = _personas;
		selected = _selected;
	
	}
	
	FEmergencePersonaListResponse(FString _json_){
		FEmergencePersonaListResponse _tmpEmergencePersonaListResponse;
		
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersonaListResponse>(
		_json_,
		&_tmpEmergencePersonaListResponse,
		0, 0);
		
		personas = _tmpEmergencePersonaListResponse.personas;
		selected = _tmpEmergencePersonaListResponse.selected;
	}
	
};

USTRUCT(BlueprintType)
struct FEmergencePersonaResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEmergencePersona persona;

	FEmergencePersonaResponse() {};

	FEmergencePersonaResponse(FEmergencePersona _persona) {
		persona = _persona;
	}

	FEmergencePersonaResponse(FString _json_) {
		FEmergencePersona _tmpEmergencePersona;

		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergencePersona>(
			_json_,
			&_tmpEmergencePersona,
			0, 0);

		persona = _tmpEmergencePersona;
	}
};