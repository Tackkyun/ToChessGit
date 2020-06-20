// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangePlayerHPDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangedMoneyDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangedEXPDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangedLevelDelegate);

/**
 * 
 */
UCLASS()
class TOCHESS_API AGamePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AGamePlayerState();

	virtual void BeginPlay() override;

	UFUNCTION()
	void AfterConnect();

public:
	UPROPERTY(BlueprintAssignable, Category = "GamePlayerState")
	FChangedMoneyDelegate changedMoneyDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GamePlayerState")
	FChangedEXPDelegate changedEXPDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GamePlayerState")
	FChangedLevelDelegate changedLevelDelegate;
	UPROPERTY(BlueprintAssignable, Category = "PlayerCharacterBase")
	FChangePlayerHPDelegate changePlayerHPDelegate;


	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	void CallMoneyDelegate();
	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	void CallEXPDelegate();
	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	void CallLevelDelegate();

private:
	UPROPERTY(replicated, ReplicatedUsing = CallMoneyDelegate)
	int32 m_money;
	UPROPERTY(replicated, ReplicatedUsing = CallLevelDelegate)
	int32 m_level;
	UPROPERTY(replicated, ReplicatedUsing = CallEXPDelegate)
	int32 m_expValue;
	UPROPERTY(replicated, ReplicatedUsing = ChangedPlayerHP)
	int32 m_playerHP;

	//per 1
	int32 interest;
	int32 maxInterest;
	
	UDataTable* m_playerDT;

	AGamePlayerState* preMatching;

	void InitData();



public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void ChangedPlayerHP();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	int32 GetPlayerHP();

	//Use for Client.
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "PlayerCharacterBase")
	void Server_AddPlayerHP(int32 _value);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_AddEXP(int32 _value);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_BuyEXP();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_AddMoney(int32 _value);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_DefaultAddMoney();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_ApplyInterest();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerState")
	void Server_BindInterest();

	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	int32 GetMoney();
	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	int32 GetLevel();
	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	int32 GetEXPValue();

	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	void SetPreMachingPlayer(AGamePlayerState* _player);
	UFUNCTION(BlueprintCallable, Category = "GamePlayerState")
	AGamePlayerState* GetPreMachingPlayer();
};
