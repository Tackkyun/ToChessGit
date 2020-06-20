// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/DeployObject/TileBase.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "UnrealNetwork.h"

ATileBase::ATileBase()
{
	//bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->Mobility = EComponentMobility::Stationary;

	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	collision->SetupAttachment(mesh);
	collision->Mobility = EComponentMobility::Stationary;
	collision->SetRelativeScale3D(mesh->GetRelativeTransform().GetScale3D() * 1.58f);

	spawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPosition"));
	spawnPosition->SetupAttachment(RootComponent);
	spawnPosition->Mobility = EComponentMobility::Stationary;

	coord = FIntPoint(-1, -1);
	bOwnerDeploy = false;
}

void ATileBase::SetCoord(int32 _x, int32 _y)
{
	coord = FIntPoint(_x, _y);
}

FIntPoint ATileBase::GetCoord()
{
	return coord;
}

bool ATileBase::GetOwnerDeploy()
{
	return bOwnerDeploy;
}

void ATileBase::SetOwnerDeploy(bool _b)
{
	bOwnerDeploy = _b;
}

void ATileBase::SetOwnerCluster(ATileCluster* _owner)
{
	ownerCluster = _owner;
}

bool ATileBase::IsDeploy(APlayerCharacterBase* _playerCharacter)
{
	if (ownerCluster->GetOwner() == _playerCharacter)
	{
		if (GetOwnerDeploy())
		{
			return true;
		}

		return false;
	}
	else
	{
		if (GetOwnerDeploy())
		{
			return false;
		}

		return true;
	}

	return false;
}

void ATileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATileBase, characterSlot);
}

FTransform ATileBase::GetSpawnPosition()
{
	return spawnPosition->GetRelativeTransform() * GetTransform();
}

