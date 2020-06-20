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

	//gamestate���� round Ȯ��.
	//datatable����.
	//AI�� �´ٸ�, �۵�.
	
	//ai data ������ ���� ���� ����.
	//�ڽ��� characterManager�� ���.
	//exit�� exitDelegate�� ���� ����.
	if (HasAuthority())
	{
		int32 currentRound = Cast<AMainGameState>(GetWorld()->GetGameState())->GetGameRound();

		FDTRoundData* roundData = UDataTableManager::GetInst()->GetRoundData()->FindRow<FDTRoundData>(FName(*FString::FromInt(currentRound)), FString(""));

		//����ó��
		if (roundData == nullptr ||
			roundData->Type == "Player")
		{
			return;
		}

		//GetTileCluster()->enemyPlayer = UGameplayStatics::GetPlayerControllerID(this);

		//������ �ؼ�
		FString unitsData = roundData->Unit;
		TArray<FSimpleUnitData> unitsNum;

		int32 digit = 0;
		for (int32 i = 0; unitsData.Len() > digit; ++i)
		{
			unitsNum.Add(FSimpleUnitData());
			int32 num = 0;

			//#���ö����� �ݺ�. num�� ���.
			unitsNum[i].num = UnitDataInterpret(digit, unitsData, '#');

			//,���ö����� �ݺ�. x�� ���.
			unitsNum[i].x = UnitDataInterpret(digit, unitsData, ',');

			//- or null ���ö����� �ݺ�. y�� ���.
			unitsNum[i].y = UnitDataInterpret(digit, unitsData, '-');
		}

		//Create and Spawn Unit
		for (int i = 0; i < unitsNum.Num(); ++i)
		{
			FUpgradeStruct tempResult = FUpgradeStruct();

			GetCharacterManager()->SpawnCharacter(unitsNum[i].num, EDeployMode::FieldTile, 1, tempResult);

			//Ÿ�� �迭�� �ֱ�.
			//Ÿ�Ͽ� ��ġ.
			GetTileCluster()->DeployCharacterToTile(GetCharacterManager()->GetFieldCharacters()[i], unitsNum[i].x, unitsNum[i].y);
			GetCharacterManager()->GetFieldCharacters()[i]->AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
	}

	//exit�� ����.
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::UnsetData);
}

void AAIGamePlayerCharacter::UnsetData()
{
	
	//bind Attack
	if (HasAuthority())
	{
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Clear")->enterDelegate.RemoveDynamic(this, &APlayerCharacterBase::Attack);
	}

	//������ ����.
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

		//Ÿ�� �迭�� �ֱ�.
		//Ÿ�Ͽ� ��ġ.
		GetTileCluster()->DeployCharacterToTile(GetCharacterManager()->GetFieldCharacters()[i], GetTileCluster()->boardSize - playerAIData[i]->hasTile->GetCoord().X, GetTileCluster()->boardSize - playerAIData[i]->hasTile->GetCoord().Y);
		GetCharacterManager()->GetFieldCharacters()[i]->AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
	}


	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AAIGamePlayerCharacter::UnsetData);

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->exitDelegate.RemoveDynamic(this, &AAIGamePlayerCharacter::SetPlayerAI);
}