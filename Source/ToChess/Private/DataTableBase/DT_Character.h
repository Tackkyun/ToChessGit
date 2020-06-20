// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "DT_Character.generated.h"

USTRUCT(BlueprintType)
struct FDTCharacterStatus : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float DEF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float MOVSPD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float DMG;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float ATKSPD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	float RNG;
};

USTRUCT(BlueprintType)
struct FDTCharacterMaterials : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString BlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString AnimBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	UTexture2D* Image2D;
};

USTRUCT(BlueprintType)
struct FDTCharacterList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	int32 Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString OneStarMaterialsNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString OneStarStatusNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString TwoStarMaterialsNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString TwoStarStatusNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString ThreeStarMaterialsNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterInfomation")
	FString ThreeStarStatusNum;
};