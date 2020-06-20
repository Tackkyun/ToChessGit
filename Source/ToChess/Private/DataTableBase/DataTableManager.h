// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ToChess/Private/DataTableBase/DT_Character.h"
#include "ToChess/Private/DataTableBase/DT_GameState.h"
#include "ToChess/Private/DataTableBase/DT_Player.h"
#include "ToChess/Private/DataTableBase/DT_RoundType.h"
#include "DataTableManager.generated.h"

/**
 * 
 */
UCLASS()
class UDataTableManager : public UObject
{
	GENERATED_BODY()
	
private:
	UDataTableManager();
	~UDataTableManager();

	//static UDataTableManager* m_Instance;

	UPROPERTY()
	UDataTable* m_CharacterStatus;
	UPROPERTY()
	UDataTable* m_CharacterStatus_TwoStar;
	UPROPERTY()
	UDataTable* m_CharacterStatus_ThreeStar;
	UPROPERTY()
	UDataTable* m_CharacterMaterials;
	UPROPERTY()
	UDataTable* m_CharacterList;
	UPROPERTY()
	UDataTable* m_GameStateLife;
	UPROPERTY()
	UDataTable* m_PlayerDT;
	UPROPERTY()
	UDataTable* m_RoundData;

public:
	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	static UDataTableManager* GetInst();

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetDTCharacterStatus(int32 _star = 1);

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetDTCharacterMaterials();

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetDTCharacterList();

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetGameStateLife();

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetPlayerDT();

	UFUNCTION(BlueprintCallable, Category = "DataTableManager")
	UDataTable* GetRoundData();
};
