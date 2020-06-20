// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "DT_RoundType.generated.h"

USTRUCT(BlueprintType)
struct FSimpleUnitData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 num;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 x;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 y;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDTRoundData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTRoundType")
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTRoundType")
	FString Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTRoundType")
	FString Unit;
};