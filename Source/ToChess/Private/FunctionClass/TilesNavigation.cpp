// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/FunctionClass/TilesNavigation.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"

UTilesNavigation::UTilesNavigation()
{

}

UTilesNavigation::~UTilesNavigation()
{

}

TArray<ATileObject*> UTilesNavigation::FindRoute(APlayerCharacterBase* _pawn, ATileObject* _start, ATileObject* _target)
{
	return _pawn->GetTileCluster()->FindRoute(_start, _target);
}


ACharacterBase* UTilesNavigation::FindTarget(APlayerCharacterBase* _pawn, FVector _pos, int _range, ACharacterBase* _self)
{
	return _pawn->GetTileCluster()->FindTarget(_pos, _range, _self);
}

bool UTilesNavigation::IsEnemiesInField(APlayerCharacterBase* _pawn)
{
	if (_pawn->GetTileCluster()->GetEnimiesNumInField(_pawn) >= 0)
	{
		return true;
	}

	return false;
}

int32 UTilesNavigation::GetEnemiesNumInField(APlayerCharacterBase* _pawn)
{
	return _pawn->GetTileCluster()->GetEnimiesNumInField(_pawn);
}