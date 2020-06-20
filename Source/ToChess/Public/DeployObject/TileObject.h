// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToChess/Public/DeployObject/TileBase.h"
#include "TileObject.generated.h"

/**
 * 
 */
UCLASS()
class TOCHESS_API ATileObject : public ATileBase
{
	GENERATED_BODY()
	
public:
	ATileObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TileObject")
	TArray<ATileObject*> nearTile;
};
