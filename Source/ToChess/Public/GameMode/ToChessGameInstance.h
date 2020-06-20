// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ToChessGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInstData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 PlayerIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString PlayerName;
};

/**
 * 
 */
UCLASS()
class TOCHESS_API UToChessGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UToChessGameInstance();

public:
	UFUNCTION(BlueprintCallable, Category = "ToChessGameInstance")
	FString GetMyIP();

	UFUNCTION(BlueprintCallable, Category = "ToChessGameInstance")
	void LoadCharacterData();

private:
	int32 playerCount;

public:
	void SetPlayerCount(int32 _count);
	UFUNCTION(BlueprintCallable, Category = "ToChessGameInstance")
	int32 GetPlayerCount();

public:
	UPROPERTY(BlueprintReadWrite, Category = "ToChessGameInstance")
	FPlayerInstData myPlayerData;

	UPROPERTY(BlueprintReadWrite, Category = "ToChessGameInstance")
	TArray<FPlayerInstData> playerDataArr;
};
