// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

DECLARE_DELEGATE(FGameStateTickDelegate);

class UGamePlayStateBase;


UENUM(BlueprintType)
enum class EFSMStateType : uint8
{
	NULLState	UMETA(DisplayName = "NULLState"),
	Enter		UMETA(DisplayName = "Enter"),
	Execute		UMETA(DisplayName = "Execute"),
	Exit		UMETA(DisplayName = "Exit")
};

UENUM(BlueprintType)
enum class EGameStateType : uint8
{
	NULLState	UMETA(DisplayName = "NULLState"),
	WaitTime	UMETA(DisplayName = "WaitTime"),
	RunGame		UMETA(DisplayName = "Execute")
};
/*
USTRUCT(BlueprintType)
struct FClinetDelegate
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFSMStateType type;

};*/

/**
 * 
 */
UCLASS()
class TOCHESS_API AMainGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMainGameState();

private:
	void InitGameState();
	
public:
	bool IsReady();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	friend UGamePlayStateBase;

	UFUNCTION()
	void AfterConnect();


private:
	bool isReady;

	UPROPERTY()
	float gameTimer;
	UPROPERTY()
	float currentStateTimer;

	FGameStateTickDelegate stateTick;

	UPROPERTY()
	TMap<FString, UGamePlayStateBase*> stateContainer;

	UPROPERTY(Replicated)
	FString currentState;
	UPROPERTY()
	int32 gameRound;

public:
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	void SetGameRound(int32 _value);
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	void SetCountNextRound();
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	int32 GetGameRound();

public:
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	UGamePlayStateBase* GetStateInContainer(FString _name);

	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	float GetGameTime();
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	float GetCurrentStateTime();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "MainGameState")
	void ResetCurrentStateTime();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "MainGameState")
	void ResetStateTime();

	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	FString GetCurrentState();

	//match
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	void UserMatching();

private:
	bool timerIsReady;

public:
	UFUNCTION(BlueprintCallable, Category = "MainGameState")
	void StartTimer();


};
