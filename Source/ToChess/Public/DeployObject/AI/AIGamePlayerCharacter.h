// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "AIGamePlayerCharacter.generated.h"

class ATileCluster;
class UCharacterManager;
class ACharacterBase;

/**
 * 
 */
UCLASS()
class TOCHESS_API AAIGamePlayerCharacter : public APlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	AAIGamePlayerCharacter();

	virtual void BeginPlay() override;

	/*
	������ ����(���� ��)
	cluster�� ����(���� ��) : characterbase ������ ���� ��.
	ĳ���� spawn(���� �� delegate ����)
	ĳ���� ����()
	*/
	UFUNCTION()
	void SetData();
	UFUNCTION()
	void UnsetData();

private:
	int32 UnitDataInterpret(int32 &_dig, FString _unitData, TCHAR _terminationCondition);

public:
	UFUNCTION()
	void SetPlayerAIData(TArray<ACharacterBase*> _unit);
	UFUNCTION()
	void SetPlayerAI();

private:
	TArray<ACharacterBase*> playerAIData;
};
