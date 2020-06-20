// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "TilesNavigation.generated.h"

class APlayerCharacterBase;

/**
 * 
 */
UCLASS()
class TOCHESS_API UTilesNavigation : public UObject
{
	GENERATED_BODY()

public:
	UTilesNavigation();
	~UTilesNavigation();

	static TArray<ATileObject*> FindRoute(APlayerCharacterBase* _pawn, ATileObject* _start, ATileObject* _target);
	static ACharacterBase* FindTarget(APlayerCharacterBase* _pawn, FVector _pos, int _range, ACharacterBase* _self);

	static bool IsEnemiesInField(APlayerCharacterBase* _pawn);
	static int32 GetEnemiesNumInField(APlayerCharacterBase* _pawn);
};
