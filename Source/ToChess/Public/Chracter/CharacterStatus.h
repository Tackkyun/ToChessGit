// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "ToChess/Private/DataTableBase/DT_Character.h"
#include "CharacterStatus.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOCHESS_API UCharacterStatus : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatus();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitData(FDTCharacterStatus* _data, int32 _star = 1);

	void ResetData();

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float hp;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float maxHP;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float defensivePower;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float moveSpeed;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float damage;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float attackSpeed;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	float range;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "UCharacterStatus")
	int32 num;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	int32 star;
	//아이템/버프 등으로 인한 증가량 확인변수 필요.

	//상태이상 delegate
	//면역정보 Array

};
