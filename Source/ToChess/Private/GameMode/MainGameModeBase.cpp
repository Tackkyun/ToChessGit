// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/MainGameModeBase.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "ToChess/Public/GameMode/GameHUD.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Public/GameMode/GamePlayerState.h"


AMainGameModeBase::AMainGameModeBase()
{	
	//PrimaryActorTick.SetTickFunctionEnable(true);
	/*
	DefaultPawnClass = APlayerCharacterBase::StaticClass();
	HUDClass = AGameHUD::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
	GameStateClass = AMainGameState::StaticClass();
	PlayerStateClass = AGamePlayerState::StaticClass();
	*/
	//SpectatorClass;
}

void AMainGameModeBase::BeginPlay()
{

}

