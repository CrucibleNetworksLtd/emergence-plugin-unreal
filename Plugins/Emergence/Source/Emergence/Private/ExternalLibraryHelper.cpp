// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalLibraryHelper.h"

FString UExternalLibraryHelper::WideToStringCleanup(char* WeirdChars) {
	FString WideString = FString::Format(TEXT("{0}"), {FStringFormatArg(WeirdChars)});
	FString ReturnString;
	for (int i = 0; i < (int)WideString.Len() * 2; i++) {
		ReturnString.AppendChar((TCHAR)WideString[i]);
	}

	return ReturnString;
}