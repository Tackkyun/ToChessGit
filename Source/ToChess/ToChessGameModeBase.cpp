// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChessGameModeBase.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "ToChess/Public/GameMode/GameHUD.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Public/GameMode/GamePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString
#include "Kismet/GameplayStatics.h"
#include "ToChess/Public/GameMode/ToChessGameInstance.h"
#include "ToChess/Public/FunctionClass/MainGameFunctionLibrary.h"


AToChessGameModeBase::AToChessGameModeBase()
{
	//PrimaryActorTick.SetTickFunctionEnable(true);

	bAllowTickBeforeBeginPlay = false;
	//bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = true;

	UE_LOG(LogTemp, Display, TEXT("************MainGame creater!"));
	/*
	DefaultPawnClass = APlayerCharacterBase::StaticClass();
	HUDClass = AGameHUD::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
	GameStateClass = AMainGameState::StaticClass();
	PlayerStateClass = AGamePlayerState::StaticClass();*/
	//SpectatorClass;

	isServerReady = false;
	allClientsReady = false;
	maxSReadyCount = 1;
}

void AToChessGameModeBase::StartPlay()
{
	Super::StartPlay();

}

void AToChessGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("***********MainGame BeginPlay!"));

	/*for (int i = 0; i < GetWorld()->GetGameState()->PlayerArray.Num(); ++i)
	{
		UE_LOG(LogTemp, Display, TEXT("***********broadcast postLogin"));
		Cast<APlayerCharacterBase>(Cast<AGamePlayerController>(GetWorld()->GetGameState()->PlayerArray[i]->GetOwner())->GetPawn())->Client_BroadCastPostLogin();
	}*/
}

void AToChessGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AToChessGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Display, TEXT("PostLogin"));

	for (int i = 0; i < GetWorld()->GetGameState()->PlayerArray.Num(); ++i)
	{
		if (Cast<AGamePlayerController>(GetWorld()->GetGameState()->PlayerArray[i]->GetOwner())->GetPawn() != nullptr)
		{
			Cast<APlayerCharacterBase>(Cast<AGamePlayerController>(GetWorld()->GetGameState()->PlayerArray[i]->GetOwner())->GetPawn())->Client_BroadCastPostLogin();
		}
	}

}


AActor* AToChessGameModeBase::SetPlayerStart(AGamePlayerController* _player)
{
	UE_LOG(LogTemp, Display, TEXT("SetPlayerStart"));

	//예외처리
	if (startedPlayers.Find(_player) != INDEX_NONE)
	{
		return nullptr;
	}

	startedPlayers.AddUnique(_player);

	APlayerStart* hasStartPoint = nullptr;
	ATileCluster* near = nullptr;

	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), outActors);

	for (int i = 0; i < outActors.Num(); ++i)
	{
		if (!(Cast<APlayerStart>(outActors[i])->PlayerStartTag == TEXT("Assign")))
		{
			Cast<APlayerStart>(outActors[i])->PlayerStartTag = TEXT("Assign");
			hasStartPoint = Cast<APlayerStart>(outActors[i]);

			break;
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATileCluster::StaticClass(), outActors);

	for (int i = 0; i < outActors.Num(); ++i)
	{
		if (near != nullptr)
		{
			if (FVector::Dist2D(hasStartPoint->GetActorLocation(), near->GetActorLocation()) > 
				FVector::Dist2D(hasStartPoint->GetActorLocation(), outActors[i]->GetActorLocation()))
			{
				near = Cast<ATileCluster>(outActors[i]);
			}
			
		}
		else
		{
			near = Cast<ATileCluster>(outActors[i]);
		}
	}

	UClass* tempClass = LoadObject<UClass>(this, TEXT("Blueprint'/Game/GameModes/BP_PlayerCharacter.BP_PlayerCharacter_C'"));
	
	FActorSpawnParameters tempParam = FActorSpawnParameters();
	tempParam.Owner = _player;
	APlayerCharacterBase* createdCharacter = GetWorld()->SpawnActor<APlayerCharacterBase>(tempClass, near->GetPlayerPosition(), tempParam);

	_player->Possess(createdCharacter);

	if (_player->GetPawn() != nullptr)
	{
		createdCharacter->SetCluster(near);
		near->SetOwner(createdCharacter);
		near->SetOwnerPlayer(createdCharacter);

		_player->Client_afterSetPositionDelegate();

		return hasStartPoint;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("SetPlayerStart in TochessGameModeBase. Player controlled pawn was null"));
		//error
	}

	return nullptr;
}

void AToChessGameModeBase::ObjectReady(UObject* _object)
{
	ReadyContainer.AddUnique(_object);

	if (ReadyContainer.Num() >= maxSReadyCount)
	{
		if (allClientsReady)
		{
			CallAfterReady();
		}
		else
		{
			isServerReady = true;
		}
	}
}

void AToChessGameModeBase::ClientReady(APlayerController* _player)
{
	ReadyPlayers.AddUnique(_player);

	if (ReadyPlayers.Num() >= Cast<UToChessGameInstance>(GetGameInstance())->GetPlayerCount())
	{
		if (isServerReady)
		{
			CallAfterReady();
		}
		else
		{
			allClientsReady = true;
		}
	}
}

void AToChessGameModeBase::CallAfterReady()
{
	afterReadyDelegate.Broadcast();

	for (int i = 0; i < GetNumPlayers(); ++i)
	{
		UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), i)->CallAfterReadyforServer();
	}
}