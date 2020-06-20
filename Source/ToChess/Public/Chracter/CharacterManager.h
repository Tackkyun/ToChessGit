// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Core.h"
#include "UObject/NoExportTypes.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "Components/ActorComponent.h"
#include "Engine/NetDriver.h"
#include "CharacterManager.generated.h"

class APlayerCharacterBase;

UENUM(BlueprintType)
enum class EDeployMode : uint8
{
	None			UMETA(DisplayName = "None"),
	WaitingSpot		UMETA(DisplayName = "WaitingSpot"),
	FieldTile		UMETA(DisplayName = "FieldTile"),
	Auto			UMETA(DisplayName = "Auto")
};

USTRUCT(BlueprintType)
struct FUpgradeStruct
{
	GENERATED_BODY()

	FUpgradeStruct()
	{
		success = false;
	}

	UPROPERTY()
	TArray<ACharacterBase*> upgradeTarget;
	UPROPERTY()
	FIntPoint priorityCoord;
	UPROPERTY()
	bool success;
	UPROPERTY()
	EDeployMode deployMode;
};

/**
 * 
 */
UCLASS()
class TOCHESS_API UCharacterManager : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UCharacterManager();

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "CharacterManager")
	APlayerCharacterBase* ownerPawn;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void Server_SetOwnerPawn(APlayerCharacterBase* _ownerPawn);

	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	void SetOwnerPawn(APlayerCharacterBase* _ownerPawn);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterManager")
	TArray<ACharacterBase*> charactersInTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterManager")
	TArray<ACharacterBase*> charactersInWaitingSpot;

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	TArray<ACharacterBase*> GetCharactersInTiles();
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	TArray<ACharacterBase*> GetCharactersInSpot();

	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	int32 GetAliveCharacterCount();
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	bool IsWaitingSpotFull();

public:
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void AddInTile(ACharacterBase* _character);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void RemoveInTile(ACharacterBase* _character);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void RemoveAllCharacterInTile();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "CharacterManager")
	void DestroyAllCharacterInTile();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void AddInWaitingSpot(ACharacterBase* _character);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterManager")
	void RemoveInWaitingSpot(ACharacterBase* _character);

	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	TArray<ACharacterBase*> GetFieldCharacters();
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	int GetFieldAliveNum();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "CharacterManager")
	void SpawnCharacter(int32 _num, EDeployMode _deploySpot, int32 _star, FUpgradeStruct _info);
	//void SpawnCharacter(int32 _num, EDeployMode _deploySpot = EDeployMode::WaitingSpot, int32 _star = 1);

	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	FUpgradeStruct CheckUpgrade(int32 _num, int32 _star, int32 _requireCount = 3, bool _destroySame = false);
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	bool ExcuteUpgrade(int32 _num);
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	bool ExcuteUpgradeFromChar(ACharacterBase* _char);
	UFUNCTION(BlueprintCallable, Category = "CharacterManager")
	void ExcuteUpgradeAll();


	//using RPC in UObject
public:
	/*
	virtual bool IsSupportedForNetworking() const override;
	
	virtual bool IsNameStableForNetworking() const override;
	
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
	
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	*/
};
