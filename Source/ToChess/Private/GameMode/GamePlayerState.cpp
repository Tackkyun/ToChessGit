// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/GameMode/GamePlayerState.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "UnrealNetwork.h"

AGamePlayerState::AGamePlayerState()
{
	preMatching = this;

	NetUpdateFrequency = 20.0f;
}

void AGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGamePlayerState, m_money);
	DOREPLIFETIME(AGamePlayerState, m_level);
	DOREPLIFETIME(AGamePlayerState, m_expValue);
	DOREPLIFETIME(AGamePlayerState, m_playerHP);
}

void AGamePlayerState::CallMoneyDelegate()
{
	changedMoneyDelegate.Broadcast();
}

void AGamePlayerState::CallEXPDelegate()
{
	changedEXPDelegate.Broadcast();
}

void AGamePlayerState::CallLevelDelegate()
{
	changedLevelDelegate.Broadcast();
}

void AGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	//**Server Communication
	if (HasAuthority())
	{
		InitData();

		if (Cast<AMainGameState>(GetWorld()->GetGameState())->IsReady())
		{
			Server_BindInterest();
		}
	}
}

void AGamePlayerState::AfterConnect()
{
	
}

void AGamePlayerState::InitData()
{
	m_money = 40;
	m_level = 1;
	m_expValue = 0;
	m_playerHP = 100;

	interest = 10;
	maxInterest = 5;

	m_playerDT = UDataTableManager::GetInst()->GetPlayerDT();
}

void AGamePlayerState::ChangedPlayerHP()
{
	UE_LOG(LogTemp, Log, TEXT("**********hphphp"));
	changePlayerHPDelegate.Broadcast();
}

int32 AGamePlayerState::GetPlayerHP()
{
	return m_playerHP;
}

void AGamePlayerState::Server_AddPlayerHP_Implementation(int32 _value)
{
	UE_LOG(LogTemp, Display, TEXT("AddHP Start"));

	m_playerHP += _value;

	if (m_playerHP <= 0)
	{
		m_playerHP = 0;

		Cast<AGamePlayerController>(GetOwner())->Client_EndGame();
	}

	UE_LOG(LogTemp, Display, TEXT("AddHP End"));
}

void AGamePlayerState::Server_AddEXP_Implementation(int32 _value)
{
	FDTPlayer* tempData = m_playerDT->FindRow<FDTPlayer>(FName(*FString::FormatAsNumber(m_level)), FString(""));

	if (tempData == nullptr) return;

	if (tempData->DemandValue > m_expValue + _value)
	{
		m_expValue += _value;
	}
	else
	{
		++m_level;
		_value -= (tempData->DemandValue - m_expValue);
		m_expValue = 0;

		Server_AddEXP(_value);
	}

	UE_LOG(LogTemp, Display, TEXT("AddEXP"));
}

void AGamePlayerState::Server_BuyEXP_Implementation()
{
	if (GetMoney() <= 2)
	{
		Server_AddEXP(2);
	}
}

void AGamePlayerState::Server_AddMoney_Implementation(int32 _value)
{
	if (m_money + _value >= 0)
	{
		m_money += _value;
		UE_LOG(LogTemp, Log, TEXT("AddMoney"));
	}
}

void AGamePlayerState::Server_DefaultAddMoney_Implementation()
{
	Server_AddMoney(5);
}

int32 AGamePlayerState::GetMoney()
{
	return m_money;
}

int32 AGamePlayerState::GetLevel()
{
	return m_level;
}

int32 AGamePlayerState::GetEXPValue()
{
	return m_expValue;
}

void AGamePlayerState::Server_ApplyInterest_Implementation()
{
	if (m_money / interest < maxInterest)
	{
		Server_AddMoney(m_money / interest);
	}
	else
	{
		Server_AddMoney(maxInterest);
	}
}

void AGamePlayerState::Server_BindInterest_Implementation()
{
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AGamePlayerState::Server_ApplyInterest);
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &AGamePlayerState::Server_DefaultAddMoney);
}

void AGamePlayerState::SetPreMachingPlayer(AGamePlayerState* _player)
{
	preMatching = _player;
}

AGamePlayerState* AGamePlayerState::GetPreMachingPlayer()
{
	return preMatching;
}