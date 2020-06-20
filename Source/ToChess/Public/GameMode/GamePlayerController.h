// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ToChess/Public/Chracter/CharacterManager.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "Engine/AssetManager.h"
#include "GamePlayerController.generated.h"

class ATileCluster;
class AGameHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAfterReadyCDelegate);

UENUM(BlueprintType)
enum class ECursorMode : uint8
{
	None			UMETA(DisplayName = "None"),
	CharacterFollow	UMETA(DisplayName = "CharacterFollow"),
	SellCharacter	UMETA(DisplayName = "SellCharacter")
};

/**
 * 
 */
UCLASS()
class TOCHESS_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	//virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags);

	//**Cluster
	/*
public:
	UPROPERTY(Replicated)
	ATileCluster* tileCluster;

	UPROPERTY(Replicated)
	UCharacterManager* characterManager;

public:
	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	void SetCluster(ATileCluster* _cluster);

	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	ATileCluster* GetTileCluster();
	
	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	UCharacterManager* GetCharacterManager();

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerController")
	void SpawnCharacter(int32 _num);*/

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GamePlayerController")
		ECursorMode cursorMode;
	

public:
	UFUNCTION()
	void AfterConnect();

	UFUNCTION(Client, Reliable)
	void Client_EndGame();

	UFUNCTION(Server, Reliable)
	void Server_FakePostLogin();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerController")
	void Server_SetPlayerPosition();

	virtual void OnRep_PlayerState();

	//HUD가 네트워크 게임에서 2개 생성되는 문제 발생하여 사용.
private:
	AGameHUD* playerHUD;

public:
	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	AGameHUD* GetPlayerHUD();

	
	//Asset Load
public:
	void AssetLoadComplete();

public:
	TSharedPtr<FStreamableHandle> assetCharacter;
	
	//Game Ready Check
public:
	//if you want to execute after ready, connect to this.
	UPROPERTY(BlueprintAssignable, Category = "GamePlayerController")
	FAfterReadyCDelegate afterReadyDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GamePlayerController")
	FAfterReadyCDelegate afterSetPositionDelegate;

	UFUNCTION(Client, Reliable)
	void Client_afterSetPositionDelegate();

	UFUNCTION()
	void afterSetPositionDeleBroad();

private:
	TArray<UObject*> ReadyContainer;
	int32 maxCReadyCount;

public:
	//if ready complete, use this
	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	void ObjectReady(UObject* _object);

	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	void CallAfterReadyforServer();

	UPROPERTY(BlueprintReadWrite, Category = "GamePlayerController")
	bool isReady;

private:
	UFUNCTION(Server, Reliable, Category = "GamePlayerController")
	void Server_ClientReady();
	UFUNCTION(Client, Reliable, Category = "GamePlayerController")
	void Client_CallAfterReady();
};

