// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "GamePlayStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateDelegate);

/**
 * 
 */
UCLASS()
class UGamePlayStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	UGamePlayStateBase();

	UPROPERTY()
	AMainGameState* owner;
	UPROPERTY()
	FString name;
	UPROPERTY()
	int32 num;
	UPROPERTY()
	FString nextState;

	UFUNCTION()
	void SetData(AMainGameState* _owner, FString _name, int32 _num, float _exitTime);
	UFUNCTION()
	void BindThis();

	UFUNCTION()
	void Enter();
	UFUNCTION()
	void Execute();
	UFUNCTION()
	void Exit();

	UPROPERTY(BlueprintAssignable, Category = "MainGameState")
	FStateDelegate enterDelegate;
	UPROPERTY(BlueprintAssignable, Category = "MainGameState")
	FStateDelegate executeDelegate;
	UPROPERTY(BlueprintAssignable, Category = "MainGameState")
	FStateDelegate exitDelegate;

	UPROPERTY()
	float exitTime;
	UPROPERTY()
	EFSMStateType currentState;
};
