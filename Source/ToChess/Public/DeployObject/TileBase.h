// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToChess/Public/DeployObject/DeployObjectBase.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "TileBase.generated.h"

class APlayerCharacterBase;
class ATileCluster;

/**
 * 
 */
UCLASS()
class ATileBase : public ADeployObjectBase
{
	GENERATED_BODY()
	
public:
	ATileBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Mesh"), Category = "ATileBase")
		UStaticMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Collision"), Category = "ATileBase")
		UBoxComponent* collision;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Collision"), Category = "ATileBase")
		ACharacterBase* characterSlot;

	//Character Spawn and Stay Position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ATileBase")
		USceneComponent* spawnPosition;

	UFUNCTION(BlueprintCallable, Category = "ATileBase")
	FTransform GetSpawnPosition();

private:
	bool bOwnerDeploy;
	ATileCluster* ownerCluster;

public:
	bool GetOwnerDeploy();
	void SetOwnerDeploy(bool _b);

	void SetOwnerCluster(ATileCluster* _owner);

	bool IsDeploy(APlayerCharacterBase* _playerCharacter);

	//coordinate
private:
	UPROPERTY()
	FIntPoint coord;

public:
	UFUNCTION()
	void SetCoord(int32 _x, int32 _y);

	UFUNCTION()
	FIntPoint GetCoord();
};
