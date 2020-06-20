// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/ToChessGameInstance.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Sockets/Public/IPAddress.h"
#include "Engine/AssetManager.h"


UToChessGameInstance::UToChessGameInstance() : playerCount(2)
{

}

FString UToChessGameInstance::GetMyIP()
{
	bool bCanBind;
	TSharedRef<FInternetAddr> myIP = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);

	return myIP->ToString(false);
}

void UToChessGameInstance::LoadCharacterData()
{
	
}

void UToChessGameInstance::SetPlayerCount(int32 _count)
{
	playerCount = _count;
}

int32 UToChessGameInstance::GetPlayerCount()
{
	return playerCount;
}