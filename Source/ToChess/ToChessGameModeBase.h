// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToChessGameModeBase.generated.h"

class AGamePlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAfterReadyDelegate);

/**
 * 
 */
UCLASS()
class TOCHESS_API AToChessGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AToChessGameModeBase();

	virtual void StartPlay() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer);

public:
	UFUNCTION()
	AActor* SetPlayerStart(AGamePlayerController* _player);

private:
	TArray<AGamePlayerController*> startedPlayers;


	//Game Ready Check
public:
	//if you want to execute after ready than connect to this.
	UPROPERTY(BlueprintAssignable, Category = "ToChessGameModeBase")
	FAfterReadyDelegate afterReadyDelegate;

private:
	UPROPERTY()
	TArray<UObject*> ReadyContainer;
	UPROPERTY()
	TArray<APlayerController*> ReadyPlayers;
	UPROPERTY()
	bool isServerReady;
	UPROPERTY()
	bool allClientsReady;

	UPROPERTY()
	int32 maxSReadyCount;

public:
	UFUNCTION(BlueprintCallable, Category = "ToChessGameModeBase")
	void ObjectReady(UObject* _object);
	UFUNCTION(BlueprintCallable, Category = "ToChessGameModeBase")
	void ClientReady(APlayerController* _player);
	
private:
	UFUNCTION()
	void CallAfterReady();
};
