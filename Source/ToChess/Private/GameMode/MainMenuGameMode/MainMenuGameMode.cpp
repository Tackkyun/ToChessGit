// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/MainMenuGameMode/MainMenuGameMode.h"
#include "Windows/WindowsPlatformProcess.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString
#include "Runtime/Launch/Public/LaunchEngineLoop.h"
#include "Engine/World.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("BeginPlay!"));
	
	//UE_LOG(LogTemp, Display, TEXT("pipe : %s"), *FPlatformProcess::ReadPipe(readPipe));

	//TEST CODE
	//if GamePlayServer
	/*
	if (GetPort() != 7777)
	{
		ExecuteGamePlayThread();
	}
	else
	{
		ExecuteLobbyThread(8);
	}*/

	currentJoinPlayer = 0;
	totalPlayerCount = 0;

	if (GetPort() != 7777)
	{
		ExecuteGamePlayThread();
	}
}

bool AMainMenuGameMode::ExecuteLobbyThread(int32 _playerCount)
{
	if (GetPort() == 7777)
	{
		FLobbyServerThread* tempThread = new FLobbyServerThread(this, &lock_LobbyServer, _playerCount);
		lobbyServerThread = FRunnableThread::Create(tempThread, TEXT("LobbyServerThread"));

		return true;
	}

	return false;
}

bool AMainMenuGameMode::ExecuteGamePlayThread()
{
	if (GetPort() != 7777)
	{
		FGamePlayServerThread* tempThread = new FGamePlayServerThread(this, &totalPlayerCount);
		gamePlayServerThread = FRunnableThread::Create(tempThread, TEXT("GamePlayServerThread"));

		return true;
	}

	return false;
}

int32 AMainMenuGameMode::GetPort()
{
	return GetWorld()->URL.Port;
}

int32 AMainMenuGameMode::GetNumFromProcessID(uint32 _processID)
{
	for (int i = 0; i < gamePlayServer.Num(); ++i)
	{
		if (gamePlayServer[i].processID == _processID)
		{
			return i;
		}
	}

	return -1;
}

int32 AMainMenuGameMode::GetNumFromPortNum(int32 _port)
{
	for (int i = 0; i < gamePlayServer.Num(); ++i)
	{
		if (gamePlayServer[i].port == _port)
		{
			return i;
		}
	}

	return -1;
}

bool AMainMenuGameMode::AddServerData(FGamePlayServerNode _data)
{
	if (GetNumFromPortNum(_data.port) == -1)
	{
		gamePlayServer.Add(_data);
		return true;
	}

	return false;
}

bool AMainMenuGameMode::RemoveServerData(int32 _port)
{
	int num = GetNumFromPortNum(_port);

	if (num != -1)
	{
		gamePlayServer.RemoveAt(num);
		return true;
	}

	return false;
}

int32 AMainMenuGameMode::IncreaseCurrentJoinPlayer()
{
	//InterlockedExchangeAdd((LPLONG)currentJoinPlayer, 1);
	return ++currentJoinPlayer;
}

bool AMainMenuGameMode::IsAllPlayerArrive()
{
	return currentJoinPlayer == totalPlayerCount;
}

void AMainMenuGameMode::JoinOrderToPlayers(int32 _port)
{
	joinOrderDelegate.Broadcast(_port);
}




//LobbyServer thread

FLobbyServerThread::FLobbyServerThread(AMainMenuGameMode* _owner, std::mutex* _lock, int32 _playerCount) : StopCounter(0), processID(0), childPort(-1), maxPlayerCount(8), waitReceive(false)
{
	owner = _owner;
	lock_GameMode = _lock;
	playerCount = _playerCount;
}

FLobbyServerThread::~FLobbyServerThread()
{

}

bool FLobbyServerThread::Init()
{
	if (owner->GetPort() == 7777)
	{
		UE_LOG(LogTemp, Log, TEXT("Init LobbyServerThread Success"));
		return true;
	}

	if (playerCount < 2 || playerCount > maxPlayerCount)
	{
		UE_LOG(LogTemp, Error, TEXT("Init LobbyServerThread. playercount is wrong"));
		return false;
	}

	return false;
}

uint32 FLobbyServerThread::Run()
{
	//excute GameServer and Get Server Port
	UE_LOG(LogTemp, Log, TEXT("LobbyServerThread Run"));
	ExecuteServer();
	UE_LOG(LogTemp, Log, TEXT("LobbyServerThread Run2"));

	while (StopCounter.GetValue() == 0)
	{
		FPlatformProcess::Sleep(1);

		if (waitReceive)
		{
			//종료대기.
			FString recv = FPlatformProcess::ReadPipe(readPipe);

			if (recv.Len() > 0)
			{

			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("LobbyServerThread Send"));
			//플레이어 정보 전송.
			FPlatformProcess::WritePipe(writePipe, FString::FromInt(playerCount));
			waitReceive = true;

			FPlatformProcess::Sleep(1);
			//유저들에게 서버로 접속하라!
			lock_GameMode->lock();
			owner->JoinOrderToPlayers(childPort);
			lock_GameMode->unlock();
		}
	}

	return 0;
}

void FLobbyServerThread::Stop()
{
	StopCounter.Increment();
}

void FLobbyServerThread::Exit()
{

}

bool FLobbyServerThread::ExecuteServer()
{
	//SetPipe
	//my pipe
	readPipe = nullptr;
	writePipe = nullptr;

	//child pipe
	chReadPipe = nullptr;
	chWritePipe = nullptr;
	
	if (!FPlatformProcess::CreatePipe(chReadPipe, writePipe) || !FPlatformProcess::CreatePipe(readPipe, chWritePipe))
	{
		UE_LOG(LogTemp, Error, TEXT("ExecuteGamePlayerServer() CreatePipe was failed"));
		return false;
	}

	//Set Process Param
	TCHAR tempParam[128] = L"";
	wsprintf(tempParam, (L"-log %d %d"), (HANDLE)chReadPipe,(HANDLE)chWritePipe);

	childHandle = FPlatformProcess::CreateProc(TEXT("D:\\ForNetwork\\EXE\\WindowsNoEditor\\ToChess\\Binaries\\Win64\\ToChessServer.exe"), tempParam, true, false, false, &processID, 0, nullptr, nullptr);
	//childHandle = FPlatformProcess::CreateProc(TEXT("D:\\ForNetwork\\EXE\\WindowsServer\\ToChessServer.exe"), tempParam, true, false, false, &processID, 0, nullptr, nullptr);

	FPlatformProcess::ClosePipe(chReadPipe, chWritePipe);

	//TEST
	//FPlatformProcess::WritePipe(writePipe, "ur child");

	//GetPort
	FString recv;
	while (recv.Len() <= 0)
	{
		recv = FPlatformProcess::ReadPipe(readPipe);
	}
	UE_LOG(LogTemp, Log, TEXT("GetPort : %s"), recv.GetCharArray().GetData());

	childPort = FCString::Atoi(*recv);


	//SetData.
	FGamePlayServerNode tempNode;

	tempNode.port = childPort;
	tempNode.isStarted = true;
	tempNode.procHandle = childHandle;
	tempNode.processID = processID;
	tempNode.readPipe = readPipe;
	tempNode.writePipe = writePipe;

	//AddArray
	lock_GameMode->lock();
	owner->AddServerData(tempNode);
	lock_GameMode->unlock();

	UE_LOG(LogTemp, Log, TEXT("ExecuteServer() success."));

	//오픈레벨 받을 때 까지 정지.


	return true;
}






//GamePlayServer thread

FGamePlayServerThread::FGamePlayServerThread(AMainMenuGameMode* _owner, int32* _totalPlayerCount) : StopCounter(0), processID(0), waitReceive(true)
{
	owner = _owner;
	totalPlayerCount = _totalPlayerCount;
}

FGamePlayServerThread::~FGamePlayServerThread()
{

}

bool FGamePlayServerThread::Init()
{
	if (__argc >= 4)
	{
		//GetHandle
		readPipe = (HANDLE)_atoi64(__argv[2]);
		writePipe = (HANDLE)_atoi64(__argv[3]);

		UE_LOG(LogTemp, Log, TEXT("Init GamePlayerServerThread Success."));

		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("__argc count was wrong"));

	return true;
}

uint32 FGamePlayServerThread::Run()
{
	UE_LOG(LogTemp, Log, TEXT("Start GamePlayServer Run."));
	FPlatformProcess::WritePipe(writePipe, FString::FromInt(owner->GetPort()));
	UE_LOG(LogTemp, Log, TEXT("Send LobbyServer"));
	FPlatformProcess::Sleep(1);

	//FString recvt = FPlatformProcess::ReadPipe(readPipe);
	//UE_LOG(LogTemp, Log, TEXT("ReadPipe : %s"), recvt.GetCharArray().GetData());

	while (StopCounter.GetValue() == 0)
	{
		FPlatformProcess::Sleep(0.5);

		if (waitReceive)
		{
			FString recv = FPlatformProcess::ReadPipe(readPipe);

			if (recv.Len() > 0)
			{
				//플레이어 전송 받음.
				*totalPlayerCount = FCString::Atoi(recv.GetCharArray().GetData());
				waitReceive = false;

				UE_LOG(LogTemp, Log, TEXT("Receive playerCount data : %d"), *totalPlayerCount);

				if (*totalPlayerCount < 1 || *totalPlayerCount > 10)
				{
					UE_LOG(LogTemp, Error, TEXT("Received userCount data was wrong. playercount range should be 1 ~ 10"));
				}
			}
		}
		else
		{
			
			//조건이 되면 종료 전송.

		}
	}

	return 0;
}

void FGamePlayServerThread::Stop()
{
	StopCounter.Increment();
}

void FGamePlayServerThread::Exit()
{

}
