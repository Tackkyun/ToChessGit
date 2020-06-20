// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/DeployObject/AI/AIGamePlayerCharacter.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/Chracter/CharacterManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ToChess/Public/Chracter/CharacterBase.h"

AAIGamePlayerCharacter::AAIGamePlayerCharacter()
{
	bReplicates = true;
	useBind = false;
	isPlayer = false;
}

void AAIGamePlayerCharacter::BeginPlay()
{
	SpawnDefaultController();

	Super::BeginPlay();
	/*
	SetOwner(GetController());

	characterManager = NewObject<UCharacterManager>(this, UCharacterManager::StaticClass());
	characterManager->Server_SetOwnerPawn(this);
	*/
	if (HasAuthority())
	{
		characterManager->Server_SetOwnerPawn(this);
	}

	//bind
	if (HasAuthority())
	{
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->enterDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::SetData);
	}
}

void AAIGamePlayerCharacter::SetData()
{
	if (GetCharacterManager()->GetFieldCharacters().Num() > 0)
	{
		return;
	}

	//bind Attack
	if (HasAuthority())
	{
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Clear")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::Attack);
	}
	/*
	if (!HasAuthority())
	{
		return;
	}*/

	//gamestate에서 round 확인.
	//datatable접근.
	//AI가 맞다면, 작동.
	
	//ai data 정보에 따라 유닛 생성.
	//자신의 characterManager에 기록.
	//exit의 exitDelegate에 삭제 연결.
	if (HasAuthority())
	{
		int32 currentRound = Cast<AMainGameState>(GetWorld()->GetGameState())->GetGameRound();

		FDTRoundData* roundData = UDataTableManager::GetInst()->GetRoundData()->FindRow<FDTRoundData>(FName(*FString::FromInt(currentRound)), FString(""));

		//예외처리
		if (roundData == nullptr ||
			roundData->Type == "Player")
		{
			return;
		}

		//GetTileCluster()->enemyPlayer = UGameplayStatics::GetPlayerControllerID(this);

		//데이터 해석
		FString unitsData = roundData->Unit;
		TArray<FSimpleUnitData> unitsNum;

		int32 digit = 0;
		for (int32 i = 0; unitsData.Len() > digit; ++i)
		{
			unitsNum.Add(FSimpleUnitData());
			int32 num = 0;

			//#나올때까지 반복. num에 기록.
			unitsNum[i].num = UnitDataInterpret(digit, unitsData, '#');

			//,나올때까지 반복. x에 기록.
			unitsNum[i].x = UnitDataInterpret(digit, unitsData, ',');

			//- or null 나올때까지 반복. y에 기록.
			unitsNum[i].y = UnitDataInterpret(digit, unitsData, '-');
		}

		//Create and Spawn Unit
		for (int i = 0; i < unitsNum.Num(); ++i)
		{
			FUpgradeStruct tempResult = FUpgradeStruct();

			GetCharacterManager()->SpawnCharacter(unitsNum[i].num, EDeployMode::FieldTile, 1, tempResult);

			//타일 배열에 넣기.
			//타일에 배치.
			GetTileCluster()->DeployCharacterToTile(GetCharacterManager()->GetFieldCharacters()[i], unitsNum[i].x, unitsNum[i].y);
			GetCharacterManager()->GetFieldCharacters()[i]->AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
	}

	//exit에 연결.
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::UnsetData);
}

void AAIGamePlayerCharacter::UnsetData()
{
	
	//bind Attack
	if (HasAuthority())
	{
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Clear")->enterDelegate.RemoveDynamic(this, &APlayerCharacterBase::Attack);
	}

	//데이터 삭제.
	if (HasAuthority())
	{
		GetCharacterManager()->DestroyAllCharacterInTile();
	}

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.RemoveDynamic(this, &AAIGamePlayerCharacter::UnsetData);

	GetTileCluster()->enemyPlayer = nullptr;
}

int32 AAIGamePlayerCharacter::UnitDataInterpret(int32& _dig, FString _unitData, TCHAR _terminationCondition)
{
	int32 num = 0;
	TCHAR burf;

	while (true)
	{
		if (_unitData.Len() <= _dig)
		{
			break;
		}

		burf = _unitData[_dig];

		if (burf == '\0')
		{
			break;
		}

		if (burf == _terminationCondition)
		{
			++_dig;
			break;
		}

		num = (num * 10) + FCString::Atoi(&burf);
		++_dig;
	}

	return num;
}


void AAIGamePlayerCharacter::SetPlayerAIData(TArray<ACharacterBase*> _unit)
{
	if (GetCharacterManager()->GetFieldCharacters().Num() > 0)
	{
		return;
	}

	//record enemy player unit data
	playerAIData = _unit;

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->exitDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::SetPlayerAI);
}

void AAIGamePlayerCharacter::SetPlayerAI()
{
	//Create and Spawn Unit
	for (int i = 0; i < playerAIData.Num(); ++i)
	{
		FUpgradeStruct tempResult = FUpgradeStruct();

		GetCharacterManager()->SpawnCharacter(playerAIData[i]->status->num, EDeployMode::FieldTile, playerAIData[i]->status->star, tempResult);

		//타일 배열에 넣기.
		//타일에 배치.
		GetTileCluster()->DeployCharacterToTile(GetCharacterManager()->GetFieldCharacters()[i], GetTileCluster()->boardSize - playerAIData[i]->hasTile->GetCoord().X, GetTileCluster()->boardSize - playerAIData[i]->hasTile->GetCoord().Y);
		GetCharacterManager()->GetFieldCharacters()[i]->AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
	}


	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::UnsetData);

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->exitDelegate.RemoveDynamic(this, &AAIGamePlayerCharacter::SetPlayerAI);
}