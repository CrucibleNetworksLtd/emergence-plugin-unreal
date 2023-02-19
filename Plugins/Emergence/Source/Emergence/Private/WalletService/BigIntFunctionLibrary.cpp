// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/BigIntFunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Math/BigInt.h"

FString UBigIntFunctionLibrary::HexToDec(const FString& HexValue) {
    const int NumBits = 256;
    TBigInt<NumBits, false> ret;
    ret.Parse(HexValue);
    FString Output;

    //its sent over as finnys aka PWei
    int64 Finny = (ret / (1000000000000000)).ToInt();
    FString FinnyText = FString::FromInt(Finny);

    return FinnyText;
}