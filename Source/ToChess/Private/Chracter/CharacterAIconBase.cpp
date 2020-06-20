// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/Chracter/CharacterAIconBase.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "ToChess/Public/FunctionClass/TilesNavigation.h"

ACharacterAIconBase::ACharacterAIconBase()
{
	PrimaryActorTick.SetTickFunctionEnable(true);

	targetEnemy = nullptr;
}

void ACharacterAIconBase::BeginPlay()
{
	Super::BeginPlay();

	targetEnemy = nullptr;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("BeginPlay"));
}

void ACharacterAIconBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACharacterAIconBase::IsEnemyInSelfRange()
{
	ACharacterBase* tempCharacterBase = Cast<ACharacterBase>(GetCharacter());

	if (tempCharacterBase != nullptr) 
	{
		targetEnemy = UTilesNavigation::FindTarget(
			Cast<APlayerCharacterBase>(Cast<ACharacterBase>(GetOwnerChracter())->GetOwnerPawn()),
			tempCharacterBase->GetActorLocation(),
			tempCharacterBase->status->range,
			Cast<ACharacterBase>(GetCharacter()));
		if(targetEnemy != nullptr)
		{
			return true;
		}
	}

	return false;
}

bool ACharacterAIconBase::IsEnemyInRange(float _range)
{
	ACharacterBase* tempCharacterBase = Cast<ACharacterBase>(GetCharacter());

	if (tempCharacterBase != nullptr)
	{
		if (UTilesNavigation::FindTarget(Cast<APlayerCharacterBase>(Cast<ACharacterBase>(GetOwnerChracter())->GetOwnerPawn()),
			tempCharacterBase->GetActorLocation(), _range,
			Cast<ACharacterBase>(GetCharacter())) != nullptr)
		{
			return true;
		}
	}

	return false;
}

bool ACharacterAIconBase::IsEnemyInInfiniteRange()
{
	//변경할것. 필드 내 적 바로확인하도록. 네비게이션 클래스에 제작.
	ACharacterBase* tempCharacterBase = Cast<ACharacterBase>(GetCharacter());

	if (tempCharacterBase != nullptr)
	{
		if (UTilesNavigation::FindTarget(Cast<APlayerCharacterBase>(Cast<ACharacterBase>(GetOwnerChracter())->GetOwnerPawn()),
			tempCharacterBase->GetActorLocation(), 100.0f, //tempCharacterBase->status->range,
			Cast<ACharacterBase>(GetCharacter())) != nullptr)
		{
			return true;
		}
	}

	return false;
}

ACharacter* ACharacterAIconBase::GetOwnerChracter()
{
	return GetCharacter();
}

bool ACharacterAIconBase::FindTargetAndMove()
{
	if (GetCharacter())
	{
		ACharacterBase* selfCharacter = Cast<ACharacterBase>(GetCharacter());
		TArray<ATileObject*> astarRoute;

		if (selfCharacter->GetOwnerPawn())
		{
			ATileCluster* tempCluster = Cast<APlayerCharacterBase>(selfCharacter->GetOwnerPawn())->GetTileCluster();
			if (tempCluster)
			{
				ACharacterBase* target = tempCluster->FindTarget(GetCharacter()->GetActorLocation(), 99, Cast<ACharacterBase>(GetCharacter()));
				if (target == nullptr) return false;

				astarRoute = UTilesNavigation::FindRoute(Cast<APlayerCharacterBase>(Cast<ACharacterBase>(GetOwnerChracter())->GetOwnerPawn()), Cast<ATileObject>(selfCharacter->hasTile), Cast<ATileObject>(target->hasTile));
				if (astarRoute.Num() <= 0) return false;
			}
		}
		
		if (astarRoute.Num() >= 2)
		{
			astarRoute.Pop();
			Cast<ACharacterBase>(GetCharacter())->MoveStraight(Cast<ATileBase>(astarRoute.Pop()));
		}
		else if (astarRoute.Num() >= 1)
		{
			Cast<ACharacterBase>(GetCharacter())->MoveStraight(Cast<ATileBase>(astarRoute.Pop()));
		}
	}

	return true;
}

void ACharacterAIconBase::ToMove(FVector _position)
{

}

void ACharacterAIconBase::StopMove()
{
	
}

void ACharacterAIconBase::Attack()
{

}