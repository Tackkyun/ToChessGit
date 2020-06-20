// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "DT_Player.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDTPlayer : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 DemandValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 MaxUnit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 PercentCostOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 PercentCostTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 PercentCostThree;
};