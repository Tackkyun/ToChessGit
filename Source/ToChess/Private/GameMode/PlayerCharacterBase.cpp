// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/Chracter/CharacterManager.h"
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "ToChess/Public/GameMode/GamePlayerState.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraSpringArm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));

	cameraSpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	camera->AttachToComponent(cameraSpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	characterManager = CreateDefaultSubobject<UCharacterManager>(TEXT("CharacterManager"));
	if (characterManager)
	{
		characterManager->SetNetAddressable();
		characterManager->SetIsReplicated(true);
	}

	bCanSpawnCharacter = false;
	bCanUpgradeFieldCharacter = false;
	useBind = true;
	isPlayer = true;
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, tileCluster);
	DOREPLIFETIME(APlayerCharacterBase, homeCluster);
	DOREPLIFETIME(APlayerCharacterBase, characterManager);
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (isPlayer)
	{
		SetOwner(GetController());

		characterManager->SetOwnerPawn(this);

		if (useBind)
		{
			//Set CharacterSpawn Time
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("WaitTime")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OnCanSpawnCharacter);
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("WaitTime")->exitDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OffCanSpawnCharacter);
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Start")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OnCanSpawnCharacter);
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Start")->exitDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OffCanSpawnCharacter);

			//Set CharacterUpgrade in Field, Time
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("WaitTimeReady")->exitDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OnCanUpgradeFieldCharacter);
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("WaitTime")->exitDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::OffCanUpgradeFieldCharacter);


			//**Server Communication
			if (HasAuthority())
			{
				//check upgrade in stage begin
				Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("WaitTimeReady")->enterDelegate.AddUniqueDynamic(characterManager, &UCharacterManager::ExcuteUpgradeAll);
			}
		}

		if (HasAuthority())
		{
			
			Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Clear")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::Attack);
		}
	}
}

void APlayerCharacterBase::AfterConnect()
{
	
}

void APlayerCharacterBase::BindDelegateProperty()
{
	if (!HasAuthority())
	{
		
	}
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacterBase::Client_BroadCastPostLogin_Implementation()
{
	postLoginDelegate.Broadcast();
}

void APlayerCharacterBase::Attack()
{
	UE_LOG(LogTemp, Display, TEXT("Attack"));
	if (GetTileCluster()->GetOtherPlayer(this) != GetTileCluster()->GetAIPlayerCharacter())
	{
		Cast<AGamePlayerState>(GetTileCluster()->GetOtherPlayer(this)->GetPlayerState())->Server_AddPlayerHP(-GetCharacterManager()->GetAliveCharacterCount());
	}
}

void APlayerCharacterBase::OnRep_SetHomeCluster()
{
	if (isPlayer)
	{
		homeCluster->SetOwner(this);
		homeCluster->ownerPlayer = this;
	}
}

void APlayerCharacterBase::SetCluster_Implementation(ATileCluster* _cluster)
{
	if (_cluster != nullptr)
	{
		tileCluster = _cluster;
		homeCluster = _cluster;
		_cluster->ownerPlayer = this;
	}
}

void APlayerCharacterBase::Multi_SetCurrentCluster_Implementation(ATileCluster* _cluster)
{
	tileCluster = _cluster;
}

ATileCluster* APlayerCharacterBase::GetTileCluster()
{
	return tileCluster;
}

ATileCluster* APlayerCharacterBase::GetHomeTileCluster()
{
	return homeCluster;
}

UCharacterManager* APlayerCharacterBase::GetCharacterManager()
{
	return characterManager;
}

void APlayerCharacterBase::SpawnCharacter_Implementation(int32 _num)
{
	if (bCanSpawnCharacter)
	{
		if (!GetCharacterManager()->ExcuteUpgrade(_num))
		{
			FUpgradeStruct tempResult = FUpgradeStruct();

			GetCharacterManager()->SpawnCharacter(_num, EDeployMode::WaitingSpot, 1, tempResult);
		}
	}
}

void APlayerCharacterBase::SetMoveForMatch(ATileCluster* _targetCluster)
{
	if (_targetCluster == nullptr)
	{
		return;
	}

	Multi_SetCurrentCluster(_targetCluster);
	
	//bind MoveForMatch
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("PreStart")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::MoveForMatch);
}

void APlayerCharacterBase::MoveForMatch()
{
	/*
	캐릭터를 클러스터 적 위치에 배치.
	*/

	GetTileCluster()->enemyPlayer = this;

	//SetActorTransform(tileCluster->GetPlayerPosition(true));
	Multi_SetPlayerPosition(GetTileCluster()->GetPlayerPosition(true));

	//보유한 캐릭터들을 이동.
	//역배치.
	//필드
	TArray<ACharacterBase*> tempCharacter = GetCharacterManager()->GetFieldCharacters();

	for (int i = 0; i < tempCharacter.Num(); ++i)
	{
		FIntPoint tempCoord = tempCharacter[i]->hasTile->GetCoord();

		tempCharacter[i]->hasTile->characterSlot = nullptr;
		tileCluster->DeployCharacterToTile(tempCharacter[i], (tileCluster->boardSize - 1) - tempCoord.X, (tileCluster->boardSize - 1) - tempCoord.Y);
	}
	
	//대기실
	tempCharacter = GetCharacterManager()->GetCharactersInSpot();

	for (int i = 0; i < tempCharacter.Num(); ++i)
	{
		FIntPoint tempCoord = tempCharacter[i]->hasTile->GetCoord();

		tempCharacter[i]->hasTile->characterSlot = nullptr;
		tileCluster->DeployEnemyWaitingSpotAt(tempCharacter[i], tempCoord.X);
	}

	//bind exit. 캐릭터 도로 돌아가고, 유닛 recovery.
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->enterDelegate.AddUniqueDynamic(this, &APlayerCharacterBase::MoveForExit);

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("PreStart")->enterDelegate.RemoveDynamic(this, &APlayerCharacterBase::MoveForMatch);
}

void APlayerCharacterBase::MoveForExit()
{
	GetTileCluster()->enemyPlayer = nullptr;
	Multi_SetCurrentCluster(homeCluster);

	Multi_SetPlayerPosition(tileCluster->GetPlayerPosition());

	//return spotCharacters.
	TArray<ACharacterBase*> tempCharacter = GetCharacterManager()->GetCharactersInSpot();

	for (int i = 0; i < tempCharacter.Num(); ++i)
	{
		FIntPoint tempCoord = tempCharacter[i]->hasTile->GetCoord();

		tempCharacter[i]->hasTile->characterSlot = nullptr;
		tileCluster->DeployFriendlyWaitingSpotAt(tempCharacter[i], tempCoord.X);
	}

	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->enterDelegate.RemoveDynamic(this, &APlayerCharacterBase::MoveForExit);

}

void APlayerCharacterBase::Multi_SetPlayerPosition_Implementation(FTransform _t)
{
	SetActorTransform(_t);
}

bool APlayerCharacterBase::CanSpawnCharacter()
{
	return bCanSpawnCharacter;
}

void APlayerCharacterBase::OnCanSpawnCharacter()
{
	bCanSpawnCharacter = true;
}

void APlayerCharacterBase::OffCanSpawnCharacter()
{
	bCanSpawnCharacter = false;
}

bool APlayerCharacterBase::CanUpgradeFieldCharacter()
{
	return bCanUpgradeFieldCharacter;
}

void APlayerCharacterBase::OnCanUpgradeFieldCharacter()
{
	bCanUpgradeFieldCharacter = true;
}

void APlayerCharacterBase::OffCanUpgradeFieldCharacter()
{
	bCanUpgradeFieldCharacter = false;
}

void APlayerCharacterBase::CheckUnitUpgrade()
{
	TArray<ACharacterBase*> tempChar = GetCharacterManager()->GetCharactersInTiles();
	tempChar.Append(GetCharacterManager()->GetCharactersInSpot());

	for (int i = 0; i < tempChar.Num(); ++i)
	{
		if (tempChar[i] != nullptr)
		{
			GetCharacterManager()->ExcuteUpgrade(tempChar[i]->status->num);
		}
	}
}
/*
bool APlayerCharacterBase::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (characterManager != nullptr)
	{
		WroteSomething |= Channel->ReplicateSubobject(characterManager, *Bunch, *RepFlags);
	}

	return WroteSomething;
}*/