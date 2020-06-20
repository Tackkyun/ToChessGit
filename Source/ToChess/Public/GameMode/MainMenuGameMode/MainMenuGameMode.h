// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericPlatform/GenericPlatformProcess.h"
//HANDLE
#include "PreWindowsApi.h"
#include <Windows.h>
//thread
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include <mutex>

#include "MainMenuGameMode.generated.h"

class FLobbyServerThread;
class FGamePlayServerThread;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinOrderDelegate, int32, targetAddress);

USTRUCT(BlueprintType)
struct FGamePlayServerNode
{
	GENERATED_USTRUCT_BODY()

	FGamePlayServerNode() : port(-1), isStarted(false) {};

public:
	UPROPERTY(Category = "FGamePlayServerNode", EditAnywhere, BlueprintReadWrite)
	int32 port;

	UPROPERTY(Category = "FGamePlayServerNode", EditAnywhere, BlueprintReadWrite)
	bool isStarted;
	
	FProcHandle procHandle;
	uint32 processID;
	HANDLE readPipe;
	HANDLE writePipe;
};

/**
 * 
 */
UCLASS()
class TOCHESS_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay();

	//if this process isn't server, then return false
	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	bool ExecuteLobbyThread(int32 _playerCount);

private:
	//if this process is server, then return true
	UFUNCTION()
	bool ExecuteGamePlayThread();
	
public:
	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	int32 GetPort();

	//if can't find item then return -1.
	UFUNCTION()
	int32 GetNumFromProcessID(uint32 _processID);

	//if can't find item then return -1.
	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	int32 GetNumFromPortNum(int32 _port);

	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	bool AddServerData(FGamePlayServerNode _data);

	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	bool RemoveServerData(int32 _port);

	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	int32 IncreaseCurrentJoinPlayer();

	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	bool IsAllPlayerArrive();

	UFUNCTION(BlueprintCallable, Category = "AMainMenuGameMode")
	void JoinOrderToPlayers(int32 _port);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AMainMenuGameMode")
	FJoinOrderDelegate joinOrderDelegate;

private:
	UPROPERTY()
	TArray<FGamePlayServerNode> gamePlayServer;
	std::mutex lock_LobbyServer;

	FRunnableThread* lobbyServerThread;
	FRunnableThread* gamePlayServerThread;

	UPROPERTY()
	int32 currentJoinPlayer;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AMainMenuGameMode")
	int32 totalPlayerCount;
};


//LobbyServer thread
class FLobbyServerThread : public FRunnable
{
private:
	FThreadSafeCounter StopCounter;

	AMainMenuGameMode* owner;

	uint32 processID;
	int32 childPort;
	FProcHandle childHandle;
	int32 playerCount;
	int32 maxPlayerCount;

	bool waitReceive;

	std::mutex* lock_GameMode;

public:
	FLobbyServerThread(AMainMenuGameMode* _owner, std::mutex* _lock, int32 _playerCount);
	virtual ~FLobbyServerThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	bool ExecuteServer();
	
	HANDLE readPipe;
	HANDLE writePipe;
	HANDLE chReadPipe;
	HANDLE chWritePipe;
};

//LobbyServer thread. Just exist only one at server process.
class FGamePlayServerThread : public FRunnable
{
private:
	FThreadSafeCounter StopCounter;
	uint32 processID;
	int32* totalPlayerCount;

	AMainMenuGameMode* owner;

	bool waitReceive;

public:
	FGamePlayServerThread(AMainMenuGameMode* _owner, int32* _totalPlayerCount);
	virtual ~FGamePlayServerThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

private:
	HANDLE readPipe;
	HANDLE writePipe;
};