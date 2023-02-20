// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceTransaction
{

  GENERATED_BODY()

  //Address of the receiver. Null when its a contract creation transaction.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString To;

  //The transaction type.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int Type = -1;

  //either 1 (success) or 0 (failure).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  bool Status = false;

  //If this transaction was to create a contract address, then this will be the address of the newly create contract, otherwise will be null.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString ContractAddress;

  //The amount of gas used by this specific transaction alone.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int64 GasUsed;

  //The total amount of gas used when this transaction was executed in the block.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int64 CumulativeGasUsed;

  //Address of the sender.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString From;

  //Block number where this transaction was in.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int64 BlockNumber;

  //Hash of the block where this transaction was in.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString BlockHash;

  //Integer of the transactions index position in the block.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int64 TransactionIndex;

  //Hash of the transaction.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  FString TransactionHash;

  //The amount of confirmations (chain's current block number - this transaction's block number).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Transaction")
  int Confirmations = -1;

  FEmergenceTransaction() {};

  FEmergenceTransaction( FString _To, int _Type, bool _Status, FString _ContractAddress, int64 _EffectiveGasPrice, int64 _GasUsed, FString _Root, FString _From, uint64 _BlockNumber, uint64 _CumulativeGasUsed, FString _BlockHash, uint64 _TransactionIndex, FString _TransactionHash, int _Confirmations){

    To = _To;
    Type = _Type;
    Status = _Status;
    ContractAddress = _ContractAddress;
    GasUsed = _GasUsed;
    CumulativeGasUsed = _CumulativeGasUsed;
    From = _From;
    BlockNumber = _BlockNumber;
    BlockHash = _BlockHash;
    TransactionIndex = _TransactionIndex;
    TransactionHash = _TransactionHash;
    Confirmations = _Confirmations;
  }
  
  FEmergenceTransaction(FString _json_){
    FEmergenceTransaction _tmpEmergenceTransaction;
    
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceTransaction>(
		_json_,
		&_tmpEmergenceTransaction,
    0, 0);
    
    To = _tmpEmergenceTransaction.To;
    Type = _tmpEmergenceTransaction.Type;
    Status = _tmpEmergenceTransaction.Status;
    ContractAddress = _tmpEmergenceTransaction.ContractAddress;
    GasUsed = _tmpEmergenceTransaction.GasUsed;
    CumulativeGasUsed = _tmpEmergenceTransaction.CumulativeGasUsed;
    From = _tmpEmergenceTransaction.From;
    BlockNumber = _tmpEmergenceTransaction.BlockNumber;
    BlockHash = _tmpEmergenceTransaction.BlockHash;
    TransactionIndex = _tmpEmergenceTransaction.TransactionIndex;
    TransactionHash = _tmpEmergenceTransaction.TransactionHash;
    Confirmations = _tmpEmergenceTransaction.Confirmations;
  }
  
};