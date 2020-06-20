// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToChess/Public/DeployObject/TileObject.h"
#include "ToChess/Public/DeployObject/CharacterWatingSpot.h"
#include "ToChess/Public/DeployObject/AI/AIGamePlayerCharacter.h"
#include "TileCluster.generated.h"

class APlayerCharacterBase;

USTRUCT()
struct FTileNode
{
	GENERATED_BODY()

	FTileNode() : tile(nullptr), preNode(nullptr), hCost(0.0f), cost(0.0f) {};
	FTileNode(ATileObject* _tile, float _hCost, float _cost = 0.0f) : tile(_tile), preNode(nullptr), hCost(_hCost), cost(_cost) {};

	UPROPERTY()
	ATileObject* tile;

	UPROPERTY()
	ATileObject* preNode;

	float hCost;
	float cost;

	FTileNode operator = (const FTileNode& _input)
	{
		tile = _input.tile;
		preNode = _input.preNode;
		hCost = _input.hCost;
		cost = _input.cost;
		return *this;
	}

	bool operator > (const FTileNode& _target) const
	{
		return cost + hCost > _target.cost + _target.hCost;
	}
};


USTRUCT(BlueprintType)
struct FTileWidth
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ATileObject*> tile;
};

USTRUCT(BlueprintType)
struct FCharacterRecordElement
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 x;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 y;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACharacter* character;
};

UCLASS()
class TOCHESS_API ATileCluster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileCluster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called Spawn
	virtual void OnConstruction(const FTransform& Transform);

	//Components
protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* parentTile;

	//Use TileClustor like 2D Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTileWidth> tileArray;

	UPROPERTY(Replicated, VisibleAnywhere)
	USceneComponent* friendlyParentSpot;

	UPROPERTY(Replicated, VisibleAnywhere)
	USceneComponent* enemyParentSpot;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<ACharacterWatingSpot*> friendlyWaitingSpotArray;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<ACharacterWatingSpot*> enemyWaitingSpotArray;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	AAIGamePlayerCharacter* AIPlayerCharacter;

public:
	AAIGamePlayerCharacter* GetAIPlayerCharacter();

public:
	//Deploy Character in FriendlyWatingSpot. if spot array was full, then return false.
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployFriendlyWaitingSpot(ACharacterBase* _character);
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployFriendlyWaitingSpotAt(ACharacterBase* _character, int32 _x);
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployFriendlyTile(ACharacterBase* _character);
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployCharacterToTile(ACharacterBase* _character, int32 _x, int32 _y);
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployEnemyWaitingSpot(ACharacterBase* _character);
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool DeployEnemyWaitingSpotAt(ACharacterBase* _character, int32 _x);

	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool IsWaitingSpotFull();
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	bool IsWaitingSpotFullAuto(APlayerCharacterBase* _ownerPawn);

	//Parameter
public:

	UFUNCTION()
	bool IsConstructorChanged();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	int32 boardSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	float tileDistance;
	//Input Path. ex) /Game/Folder/BlueprintName.BlueprintName_C
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	TArray<FString> BlueprintPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	int32 waitingSpotSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	float spotDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	FVector basePosition;
	//Input Path. ex) /Game/Folder/BlueprintName.BlueprintName_C
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	FString spotBlueprintPath;

private:
	//Creating and Setting Tiles.(Delete Existing Components)
	void CreatingAndSettingTiles();

	void SetTileArray();

	void SetNearTile();

	void CreatingAndSettingSpots();

	void SetSpotArray();

	void BindGameState();

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TileCluster")
	void RebuildFromSettings();

public:
	UFUNCTION()
	TArray<ATileObject*> FindRoute(ATileObject* _start, ATileObject* _target);

	//Find Enemy
	UFUNCTION()
	ACharacterBase* FindTarget(FVector _pos, int _range, ACharacterBase* _self);

	UFUNCTION()
	int32 GetEnimiesNumInField(APlayerCharacterBase* _self);

	UFUNCTION()
	int32 GetFriendlyNumInField(APlayerCharacterBase* _self);

	//**Owner User
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	APlayerCharacterBase* ownerPlayer;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	APlayerCharacterBase* enemyPlayer;

	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	APlayerCharacterBase* GetOwnerPlayer();
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	void SetOwnerPlayer(APlayerCharacterBase* _player);
private:
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetOwnerPlayer(APlayerCharacterBase* _player);
public:
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	APlayerCharacterBase* GetOtherPlayer(APlayerCharacterBase* _self);

public:
	UFUNCTION(BlueprintCallable, Category = "TileCluster")
	FTransform GetPlayerPosition(bool _isEnemy = false);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	float locationCorrection;

	//Record Characters
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "TileCluster")
	TArray<FCharacterRecordElement> recordedCharacters;

	UFUNCTION()
	void RunRecord();

	UFUNCTION()
	void RecoverFromRecord();


};
