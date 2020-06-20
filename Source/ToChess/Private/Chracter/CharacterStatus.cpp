// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/Chracter/CharacterStatus.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UCharacterStatus::UCharacterStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;

	// ...
}

void UCharacterStatus::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterStatus, hp);
	DOREPLIFETIME(UCharacterStatus, maxHP);
	DOREPLIFETIME(UCharacterStatus, defensivePower);
	DOREPLIFETIME(UCharacterStatus, moveSpeed);
	DOREPLIFETIME(UCharacterStatus, damage);
	DOREPLIFETIME(UCharacterStatus, attackSpeed);
	DOREPLIFETIME(UCharacterStatus, range);
	DOREPLIFETIME(UCharacterStatus, num);
	DOREPLIFETIME(UCharacterStatus, star);
}

// Called when the game starts
void UCharacterStatus::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCharacterStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterStatus::InitData(FDTCharacterStatus* _data, int32 _star)
{
	//캐릭터 기본데이터 받아오기. 세팅으로?
	hp = _data->HP;
	maxHP = _data->HP;
	range = _data->RNG;
	damage = _data->DMG;
	defensivePower = _data->DEF;
	attackSpeed = _data->ATKSPD;
	moveSpeed = _data->MOVSPD;
	num = _data->Num;

	star = _star;
}

void UCharacterStatus::ResetData()
{
	hp = maxHP;
}