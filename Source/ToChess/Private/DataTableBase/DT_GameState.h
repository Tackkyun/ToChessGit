// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "DT_GameState.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDTGameStateLife : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTGameStateLife")
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTGameStateLife")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FDTGameStateLife")
	float NextTime;
};