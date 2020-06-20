// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "ToChess/Public/GameMode/GamePlayerState.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "ToChess/Public/Chracter/CharacterManager.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/DeployObject/AI/AIGamePlayerCharacter.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "ToChessGameModeBase.h"
#include "ToChess/Public/FunctionClass/MainGameFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AMainGameState::AMainGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	
	isReady = false;
	timerIsReady = false;

	gameRound = 1;

}

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainGameState, currentState);
}

void AMainGameState::InitGameState()
{
	if (stateContainer.Num() <= 0)
	{
		gameTimer = 0.0f;
		currentStateTimer = 0.0f;

		//Set GameState from DataTable(DTGameStateLife)
		UDataTable* stateLifeTable = UDataTableManager::GetInst()->GetGameStateLife();

		for (int i = 1; true; ++i)
		{
			FDTGameStateLife* tempData = stateLifeTable->FindRow<FDTGameStateLife>(FName(*FString::FormatAsNumber(i)), FString(""));
			if (tempData->Num <= 0)
			{
				break;
			}

			UGamePlayStateBase* tempState = NewObject<UGamePlayStateBase>(this, UGamePlayStateBase::StaticClass());
			tempState->SetData(this, tempData->Name, tempData->Num, tempData->NextTime);

			stateContainer.Add(tempData->Name, tempState);
		}

		//HardCode. GameState 첫 세팅. 수정 할 것.
		{
			FDTGameStateLife* tempData = stateLifeTable->FindRow<FDTGameStateLife>(FName(*FString::FormatAsNumber(1)), FString(""));
			UGamePlayStateBase** tempState = stateContainer.Find(tempData->Name);
			(*tempState)->BindThis();
		}

		GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AMainGameState::SetCountNextRound);
	}
}

void AMainGameState::SetGameRound(int32 _value)
{
	gameRound = _value;
}

void AMainGameState::SetCountNextRound()
{
	++gameRound;
}

int32 AMainGameState::GetGameRound()
{
	return gameRound;
}

bool AMainGameState::IsReady()
{
	return isReady;
}

void AMainGameState::BeginPlay()
{
	Super::BeginPlay();

	//**Local
	if (!IsReady())
	{
		InitGameState();
		isReady = true;
	}

	//**Bind
	if (HasAuthority())
	{
		if (Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->afterReadyDelegate.AddUniqueDynamic(this, &AMainGameState::StartTimer);
		}
	}
	else
	{
		UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0)->afterReadyDelegate.AddUniqueDynamic(this, &AMainGameState::StartTimer);
	}

	//Attaching userMatch
	if (HasAuthority())
	{
		GetStateInContainer("Ready")->enterDelegate.AddUniqueDynamic(this, &AMainGameState::UserMatching);
	}


	//Ready complete
	if (HasAuthority())
	{
		Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->ObjectReady(this);
	}
	else
	{
		UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0)->ObjectReady(this);
		/*
		//간헐적으로 이전 레벨의 PlayerController가 살아있음.
		int32 pcNum = 0;

		for (int i = 0; i <= 10; ++i)
		{
			if (Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), i)))
			{
				pcNum = i;
				break;
			}
		}

		if (Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), pcNum)))
		{
			Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), pcNum))->ObjectReady(this);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("GetPlayerController Call fail in MainGmaeState"));
		}*/
	}
}

void AMainGameState::AfterConnect()
{
	
}

void AMainGameState::UserMatching()
{
	FString roundType = UDataTableManager::GetInst()->GetRoundData()->FindRow<FDTRoundData>(FName(*FString::FromInt(GetGameRound())), FString(""))->Type;
	
	if (!HasAuthority() || roundType != "Player")
	{
		return;
	}


	//유저 리스트 불러오기.
	TArray<APlayerState*> tempPlayerArray = PlayerArray;
	
	while (tempPlayerArray.Num() >= 2)
	{
		int32 playerObjectNum = UKismetMathLibrary::RandomIntegerInRange(0, tempPlayerArray.Num() - 1);
		
		AGamePlayerState* playerObject = Cast<AGamePlayerState>(tempPlayerArray[playerObjectNum]);
		tempPlayerArray.RemoveAt(playerObjectNum);

		int32 playerSubjectNum = UKismetMathLibrary::RandomIntegerInRange(0, tempPlayerArray.Num() - 1);

		if (tempPlayerArray[playerSubjectNum] == playerObject->GetPreMachingPlayer() && tempPlayerArray.Num() > 1)
		{
			if (playerSubjectNum < (tempPlayerArray.Num() - 1))
			{
				++playerSubjectNum;
			}
			else
			{
				--playerSubjectNum;
			}
		}

		AGamePlayerState* playerSubject = Cast<AGamePlayerState>(tempPlayerArray[playerSubjectNum]);
		tempPlayerArray.RemoveAt(playerSubjectNum);

		//(multicast?) player와 매칭
		playerObject->SetPreMachingPlayer(playerSubject);
		playerSubject->SetPreMachingPlayer(playerObject);

		APlayerCharacterBase* tempPlayer = Cast<APlayerCharacterBase>(Cast<AGamePlayerController>(playerSubject->GetOwner())->GetPawn());

		if (tempPlayer != nullptr)
		{
			tempPlayer->SetMoveForMatch(Cast<APlayerCharacterBase>((Cast<AGamePlayerController>(playerObject->GetOwner())->GetPawn()))->GetTileCluster());
		}
	}

	if (tempPlayerArray.Num() >= 1)
	{
		//multicast ai player와 매칭
		//record에서 불러오기
		tempPlayerArray = PlayerArray;

		int32 playerAISubjectNum = UKismetMathLibrary::RandomIntegerInRange(0, tempPlayerArray.Num() - 1);

		APlayerCharacterBase* tempPlayer = Cast<APlayerCharacterBase>(Cast<AGamePlayerController>(tempPlayerArray[playerAISubjectNum]->GetOwner())->GetPawn());


		//캐릭터 복사.
		tempPlayer->GetTileCluster()->GetAIPlayerCharacter()->SetPlayerAIData(tempPlayer->GetCharacterManager()->GetFieldCharacters());
	}
}

void AMainGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (timerIsReady)
	{
		gameTimer += DeltaTime;
		currentStateTimer += DeltaTime;

		if (stateTick.IsBound())
		{
			stateTick.Execute();
		}
	}
}

UGamePlayStateBase* AMainGameState::GetStateInContainer(FString _name)
{
	if (!IsReady())
	{
		InitGameState();
		isReady = true;
	}

	return *(stateContainer.Find(_name));
}

float AMainGameState::GetGameTime()
{
	return gameTimer;
}

float AMainGameState::GetCurrentStateTime()
{
	return currentStateTimer;
}

void AMainGameState::ResetCurrentStateTime_Implementation()
{
	currentStateTimer = 0.0f;
}

void AMainGameState::ResetStateTime_Implementation()
{
	gameTimer = 0.0f;
	currentStateTimer = 0.0f;
}

FString AMainGameState::GetCurrentState()
{
	return currentState;
}


void AMainGameState::StartTimer()
{
	UE_LOG(LogTemp, Display, TEXT("StartTimer"));
	timerIsReady = true;
}
