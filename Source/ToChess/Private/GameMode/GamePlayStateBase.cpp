// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"

UGamePlayStateBase::UGamePlayStateBase()
{
	owner = nullptr;
	exitTime = 0.0f;
	currentState = EFSMStateType::NULLState;
}

void UGamePlayStateBase::SetData(AMainGameState* _owner, FString _name, int32 _num, float _exitTime)
{
	owner = _owner;
	name = _name;
	num = _num;
	exitTime = _exitTime;
}

void UGamePlayStateBase::BindThis()
{
	if (owner != nullptr)
	{
		owner->stateTick.Unbind();
		owner->stateTick.BindUObject(this, &UGamePlayStateBase::Enter);
	}
}

void UGamePlayStateBase::Enter()
{
	currentState = EFSMStateType::Enter;
	owner->currentState = name;

	enterDelegate.Broadcast();

	if (owner != nullptr)
	{
		owner->stateTick.Unbind();
		owner->stateTick.BindUObject(this, &UGamePlayStateBase::Execute);
	}
}

void UGamePlayStateBase::Execute()
{
	currentState = EFSMStateType::Execute;

	executeDelegate.Broadcast();

	if (owner != nullptr && owner->GetCurrentStateTime() >= exitTime)
	{
		owner->stateTick.Unbind();
		owner->stateTick.BindUObject(this, &UGamePlayStateBase::Exit);
	}
}

void UGamePlayStateBase::Exit()
{
	currentState = EFSMStateType::Exit;

	exitDelegate.Broadcast();

	UDataTable* stateLifeTable = UDataTableManager::GetInst()->GetGameStateLife();
	FDTGameStateLife* tempData = stateLifeTable->FindRow<FDTGameStateLife>(FName(*FString::FormatAsNumber(num + 1)), FString(""));

	if (tempData->Num <= 0)
	{
		tempData = stateLifeTable->FindRow<FDTGameStateLife>(FName(*FString::FormatAsNumber(1)), FString(""));
	}

	UGamePlayStateBase** tempState = owner->stateContainer.Find(tempData->Name);
	(*tempState)->BindThis();
	owner->ResetCurrentStateTime();
}

