// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceTransaction
{

  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString To;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Type;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Logs;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Status;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ContractAddress;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString EffectiveGasPrice;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString GasUsed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString CumulativeGasUsed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Root;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString From;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString BlockNumber;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString BlockHash;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString TransactionIndex;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString TransactionHash;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString LogsBloom;

  FEmergenceTransaction() {};

  FEmergenceTransaction( FString _To, FString _Type, FString _Logs, FString _Status, FString _ContractAddress, FString _EffectiveGasPrice, FString _GasUsed, FString _CumulativeGasUsed, FString _Root, FString _From, FString _BlockNumber, FString _BlockHash, FString _TransactionIndex, FString _TransactionHash, FString _LogsBloom ){

    To = _To;
    Type = _Type;
    Logs = _Logs;
    Status = _Status;
    ContractAddress = _ContractAddress;
    EffectiveGasPrice = _EffectiveGasPrice;
    GasUsed = _GasUsed;
    CumulativeGasUsed = _CumulativeGasUsed;
    Root = _Root;
    From = _From;
    BlockNumber = _BlockNumber;
    BlockHash = _BlockHash;
    TransactionIndex = _TransactionIndex;
    TransactionHash = _TransactionHash;
    LogsBloom = _LogsBloom;
  
  }
  
  FEmergenceTransaction(FString _json_){
    FEmergenceTransaction _tmpEmergenceTransaction;
    
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceTransaction>(
		_json_,
		&_tmpEmergenceTransaction,
    0, 0);
    
    To = _tmpEmergenceTransaction.To;
    Type = _tmpEmergenceTransaction.Type;
    Logs = _tmpEmergenceTransaction.Logs;
    Status = _tmpEmergenceTransaction.Status;
    ContractAddress = _tmpEmergenceTransaction.ContractAddress;
    EffectiveGasPrice = _tmpEmergenceTransaction.EffectiveGasPrice;
    GasUsed = _tmpEmergenceTransaction.GasUsed;
    CumulativeGasUsed = _tmpEmergenceTransaction.CumulativeGasUsed;
    Root = _tmpEmergenceTransaction.Root;
    From = _tmpEmergenceTransaction.From;
    BlockNumber = _tmpEmergenceTransaction.BlockNumber;
    BlockHash = _tmpEmergenceTransaction.BlockHash;
    TransactionIndex = _tmpEmergenceTransaction.TransactionIndex;
    TransactionHash = _tmpEmergenceTransaction.TransactionHash;
    LogsBloom = _tmpEmergenceTransaction.LogsBloom;
  }
  
};