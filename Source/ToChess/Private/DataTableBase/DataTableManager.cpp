// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "Engine/Engine.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UDataTableManager::UDataTableManager()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> cStatusDataTable(TEXT("DataTable'/Game/DataTables/DT_Character_Status.DT_Character_Status'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cStatusDataTableTwo(TEXT("DataTable'/Game/DataTables/DT_Character_Status_TwoStar.DT_Character_Status_TwoStar'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cStatusDataTableThree(TEXT("DataTable'/Game/DataTables/DT_Character_Status_ThreeStar.DT_Character_Status_ThreeStar'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cMaterialsDataTable(TEXT("DataTable'/Game/DataTables/DT_Character_Materials.DT_Character_Materials'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cCharacterListDataTable(TEXT("DataTable'/Game/DataTables/DT_Character_List.DT_Character_List'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cGameStateLife(TEXT("DataTable'/Game/DataTables/DT_GameState_Life.DT_GameState_Life'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cPlayerDT(TEXT("DataTable'/Game/DataTables/DT_Player_Data.DT_Player_Data'"));
	static ConstructorHelpers::FObjectFinder<UDataTable> cRoundData(TEXT("DataTable'/Game/DataTables/DT_RoundType.DT_RoundType'"));

	m_CharacterStatus = cStatusDataTable.Object;
	m_CharacterStatus_TwoStar = cStatusDataTableTwo.Object;
	m_CharacterStatus_ThreeStar = cStatusDataTableThree.Object;
	m_CharacterMaterials = cMaterialsDataTable.Object;
	m_CharacterList = cCharacterListDataTable.Object;
	m_GameStateLife = cGameStateLife.Object;
	m_PlayerDT = cPlayerDT.Object;
	m_RoundData = cRoundData.Object;
}

UDataTableManager::~UDataTableManager()
{

}

UDataTableManager* UDataTableManager::GetInst()
{
	return Cast<UDataTableManager>(GEngine->GameSingleton);
	/*
	if (m_Instance == nullptr)
	{
		m_Instance = NewObject<UDataTableManager>();

	}

	return m_Instance;*/
}

UDataTable* UDataTableManager::GetDTCharacterStatus(int32 _star)
{
	/*
	switch (_star)
	{
	case 1:
		return m_CharacterStatus;
		break;
	case 2:
		return m_CharacterStatus_TwoStar;
		break;
	case 3:
		return m_CharacterStatus_ThreeStar;
		break;
	default:
		return m_CharacterStatus;
	}*/
	
	return m_CharacterStatus;
}

UDataTable* UDataTableManager::GetDTCharacterMaterials()
{
	return m_CharacterMaterials;
}

UDataTable* UDataTableManager::GetDTCharacterList()
{
	return m_CharacterList;
}

UDataTable* UDataTableManager::GetGameStateLife()
{
	return m_GameStateLife;
}

UDataTable* UDataTableManager::GetPlayerDT()
{
	return m_PlayerDT;
}

UDataTable* UDataTableManager::GetRoundData()
{
	return m_RoundData;
}