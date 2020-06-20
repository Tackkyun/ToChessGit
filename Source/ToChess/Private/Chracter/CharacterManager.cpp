// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/Chracter/CharacterManager.h"
#include "ToChess/Private/DataTableBase/DT_Character.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Animation/AnimBlueprint.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "UnrealNetwork.h"
#include "ToChess/Public/FunctionClass/MainGameFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString


UCharacterManager::UCharacterManager()
{
	bReplicates = true;
}

void UCharacterManager::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCharacterManager::AddInTile_Implementation(ACharacterBase* _character)
{
	charactersInTile.Add(_character);
}

void UCharacterManager::RemoveInTile_Implementation(ACharacterBase* _character)
{
	charactersInTile.Remove(_character);
}

void UCharacterManager::RemoveAllCharacterInTile_Implementation()
{
	charactersInTile.Empty();
}

void UCharacterManager::DestroyAllCharacterInTile_Implementation()
{
	while (charactersInTile.Num() >= 1)
	{
		if (charactersInTile[0]->hasTile != nullptr)
		{
			charactersInTile[0]->hasTile->characterSlot = nullptr;
		}
		charactersInTile[0]->Server_DestroyThis();
	}
}

void UCharacterManager::AddInWaitingSpot_Implementation(ACharacterBase* _character)
{
	charactersInWaitingSpot.Add(_character);
}

void UCharacterManager::RemoveInWaitingSpot_Implementation(ACharacterBase* _character)
{
	charactersInWaitingSpot.Remove(_character);
}

TArray<ACharacterBase*> UCharacterManager::GetFieldCharacters()
{
	return charactersInTile;
}

TArray<ACharacterBase*> UCharacterManager::GetCharactersInSpot()
{
	return charactersInWaitingSpot;
}

TArray<ACharacterBase*> UCharacterManager::GetCharactersInTiles()
{
	return charactersInTile;
}

int32 UCharacterManager::GetAliveCharacterCount()
{
	int32 result = 0;

	for (int32 i = 0; i < GetCharactersInTiles().Num(); ++i)
	{
		if (GetCharactersInTiles()[i]->status->hp > 0.0f)
		{
			++result;
		}
	}

	return result;
}

bool UCharacterManager::IsWaitingSpotFull()
{
	if (charactersInWaitingSpot.Num() >= ownerPawn->GetTileCluster()->waitingSpotSize)
	{
		return true;
	}

	return false;
}

int UCharacterManager::GetFieldAliveNum()
{
	//살아있는 캐릭터 수
	int returnVaule = 0;

	/*
	for (ACharacterBase* i : charactersInTile)
	{
		//살아있다면
		if (1)
		{
			++returnVaule;
		}
	}*/

	return 0;
}

void UCharacterManager::Server_SetOwnerPawn_Implementation(APlayerCharacterBase* _ownerPawn)
{
	ownerPawn = _ownerPawn;
}

void UCharacterManager::SetOwnerPawn(APlayerCharacterBase* _ownerPawn)
{
	ownerPawn = _ownerPawn;
}

void UCharacterManager::SpawnCharacter_Implementation(int32 _num, EDeployMode _deploySpot, int32 _star, FUpgradeStruct _info)
{
	if (_star == 1 && IsWaitingSpotFull())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SpawnCharacterStart"));

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("SpawnCharacter"));

	FString contextString;
	//Character Data in Table
	FDTCharacterList* tempCharacterList = UDataTableManager::GetInst()->GetDTCharacterList()->FindRow<FDTCharacterList>(FName(*FString::FromInt(_num)), contextString);

	FDTCharacterStatus* tempStatusData = nullptr;
	FDTCharacterMaterials* tempMaterialData = nullptr;

	switch (_star)
	{
	case 1:
		tempStatusData = UDataTableManager::GetInst()->GetDTCharacterStatus(_star)->FindRow<FDTCharacterStatus>(FName(*(tempCharacterList->OneStarStatusNum)), contextString);
		tempMaterialData = UDataTableManager::GetInst()->GetDTCharacterMaterials()->FindRow<FDTCharacterMaterials>(FName(*(tempCharacterList->OneStarMaterialsNum)), contextString);
		break;
	case 2:
		tempStatusData = UDataTableManager::GetInst()->GetDTCharacterStatus(_star)->FindRow<FDTCharacterStatus>(FName(*(tempCharacterList->TwoStarStatusNum)), contextString);
		tempMaterialData = UDataTableManager::GetInst()->GetDTCharacterMaterials()->FindRow<FDTCharacterMaterials>(FName(*(tempCharacterList->TwoStarMaterialsNum)), contextString);
		break;
	case 3:
		tempStatusData = UDataTableManager::GetInst()->GetDTCharacterStatus(_star)->FindRow<FDTCharacterStatus>(FName(*(tempCharacterList->ThreeStarStatusNum)), contextString);
		tempMaterialData = UDataTableManager::GetInst()->GetDTCharacterMaterials()->FindRow<FDTCharacterMaterials>(FName(*(tempCharacterList->ThreeStarMaterialsNum)), contextString);
		break;
	default:
		return;
		break;
	}

	//Set Class
	UClass* tempClass;

	if (tempMaterialData->BlueprintClass.Len() > 0)
	{
		//Load BlueprintClass.
		tempClass = LoadObject<UClass>(this, *tempMaterialData->BlueprintClass);
		if (tempClass == nullptr)
		{
			tempClass = ACharacterBase::StaticClass();
		}
	}
	else
	{
		tempClass = ACharacterBase::StaticClass();
	}

	//Spawn and Set Character
	if (GetWorld() != nullptr)
	{
		FActorSpawnParameters tempParam = FActorSpawnParameters();
		tempParam.Owner = ownerPawn;
		ACharacterBase* tempCharacter = (GetWorld()->SpawnActor<ACharacterBase>(tempClass, FVector::ZeroVector, FRotator::ZeroRotator, tempParam));
		tempCharacter->SetOwner(ownerPawn);

		tempCharacter->SetStatusData(*tempStatusData, _star);

		//SetAnimBlueprint
		if (tempMaterialData->AnimBlueprint.Len() > 0)
		{
			tempClass = LoadObject<UClass>(this, *tempMaterialData->AnimBlueprint);
			tempCharacter->GetMesh()->SetAnimInstanceClass(tempClass);
		}

		switch (_deploySpot) 
		{
		case(EDeployMode::WaitingSpot):
			if (ownerPawn->GetHomeTileCluster() == ownerPawn->GetTileCluster())
			{
				AddInWaitingSpot(tempCharacter);
				ownerPawn->GetTileCluster()->DeployFriendlyWaitingSpot(tempCharacter);
			}
			else
			{
				AddInWaitingSpot(tempCharacter);
				ownerPawn->GetTileCluster()->DeployEnemyWaitingSpot(tempCharacter);
			}
			break;
		case(EDeployMode::FieldTile):
			AddInTile(tempCharacter);
			break;
		case(EDeployMode::Auto):
			if (_info.success)
			{
				if (_info.deployMode == EDeployMode::FieldTile)
				{
					AddInTile(tempCharacter);
					ownerPawn->GetTileCluster()->DeployCharacterToTile(tempCharacter, _info.priorityCoord.X, _info.priorityCoord.Y);
				}
				else if (_info.deployMode == EDeployMode::WaitingSpot)
				{
					if (ownerPawn->GetHomeTileCluster() == ownerPawn->GetTileCluster())
					{
						AddInWaitingSpot(tempCharacter);
						ownerPawn->GetTileCluster()->DeployFriendlyWaitingSpotAt(tempCharacter, _info.priorityCoord.X);
					}
					else
					{
						AddInWaitingSpot(tempCharacter);
						ownerPawn->GetTileCluster()->DeployEnemyWaitingSpotAt(tempCharacter, _info.priorityCoord.X);
					}
				}
			}
			break;
		default:
			break;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SpawnCharacterEnd"));
	return;
}

FUpgradeStruct UCharacterManager::CheckUpgrade(int32 _num, int32 _star, int32 _requireCount, bool _destroySame)
{
	FUpgradeStruct result;
	result.priorityCoord = FIntPoint(-1, -1);
	result.success = false;

	for (int i = 0; i < charactersInWaitingSpot.Num(); ++i)
	{
		if (charactersInWaitingSpot[i] == nullptr) continue;

		if (charactersInWaitingSpot[i]->status->num == _num && charactersInWaitingSpot[i]->status->star == _star)
		{
			if (result.priorityCoord.Y < 0)
			{
				result.priorityCoord = charactersInWaitingSpot[i]->hasTile->GetCoord();
			}

			result.upgradeTarget.Add(charactersInWaitingSpot[i]);
			if (result.upgradeTarget.Num() >= _requireCount)
			{
				if (_destroySame)
				{
					for (int j = 0; j < result.upgradeTarget.Num(); ++j)
					{
						result.upgradeTarget[j]->Server_DestroyThis();
					}
				}

				result.deployMode = EDeployMode::WaitingSpot;
				result.success = true;

				return result;
			}
		}
	}

	//if (bCanUpgradeFieldCharacter)
	if(Cast<APlayerCharacterBase>(GetOwner())->CanUpgradeFieldCharacter())
	{
		for (int i = 0; i < charactersInTile.Num(); ++i)
		{
			if (charactersInTile[i] == nullptr) continue;

			if (charactersInTile[i]->status->num == _num && charactersInTile[i]->status->star == _star)
			{
				//Set Position
				result.priorityCoord = charactersInTile[i]->hasTile->GetCoord();

				result.upgradeTarget.Add(charactersInTile[i]);
				if (result.upgradeTarget.Num() >= _requireCount)
				{
					if (_destroySame)
					{
						for (int j = 0; j < result.upgradeTarget.Num(); ++j)
						{
							result.upgradeTarget[j]->Server_DestroyThis();
						}
					}

					result.deployMode = EDeployMode::FieldTile;
					result.success = true;

					return result;
				}
			}
		}
	}

	return result;
}

bool UCharacterManager::ExcuteUpgrade(int32 _num)
{
	FUpgradeStruct tempResult = CheckUpgrade(_num, 1, 2, true);

	if (tempResult.success)
	{
		//2성유닛 생성
		SpawnCharacter(_num, EDeployMode::Auto, 2, tempResult);

		tempResult = CheckUpgrade(_num, 2, 3, true);

		if (tempResult.success)
		{
			SpawnCharacter(_num, EDeployMode::Auto, 3, tempResult);
			//3성유닛 생성
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UCharacterManager::ExcuteUpgradeFromChar(ACharacterBase* _char)
{
	FUpgradeStruct tempResult;

	if (_char->status->star == 2)
	{
		tempResult = CheckUpgrade(_char->status->num, 2, 3, true);

		if (tempResult.success)
		{
			SpawnCharacter(_char->status->num, EDeployMode::Auto, 3, tempResult);
			//3성유닛 생성
		}
		return true;
	}
	else
	{
		tempResult = CheckUpgrade(_char->status->num, 1, 3, true);

		if (tempResult.success)
		{
			//2성유닛 생성
			SpawnCharacter(_char->status->num, EDeployMode::Auto, 2, tempResult);

			tempResult = CheckUpgrade(_char->status->num, 2, 3, true);

			if (tempResult.success)
			{
				SpawnCharacter(_char->status->num, EDeployMode::Auto, 3, tempResult);
				//3성유닛 생성
			}
			return true;
		}
	}

	return false;
}

void UCharacterManager::ExcuteUpgradeAll()
{
	for (int i = 0; i < charactersInWaitingSpot.Num(); ++i)
	{
		if (charactersInWaitingSpot[i] != nullptr)
		{
			ExcuteUpgradeFromChar(charactersInWaitingSpot[i]);
		}
	}
}

void UCharacterManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterManager, ownerPawn);
	//DOREPLIFETIME(UCharacterManager, charactersInTile);
	//DOREPLIFETIME(UCharacterManager, charactersInWaitingSpot);
}

/*
bool UCharacterManager::IsSupportedForNetworking() const
{
	return true;
}

bool UCharacterManager::IsNameStableForNetworking() const
{
	return true;
}

bool UCharacterManager::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{

	if (AController * MyOwner = UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0))
	{
		UNetDriver* NetDriver = MyOwner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(MyOwner, Function, Parameters, OutParms, Stack, this);
			return true;
		}
	}
	return false;
}

int32 UCharacterManager::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	AController* MyOwner = UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0);
	return (MyOwner ? MyOwner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}
*/

