// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ToChess/Public/DeployObject/TileCluster.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "ToChess/ToChessGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "ToChess/Public/GameMode/GameHUD.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString



AGamePlayerController::AGamePlayerController(const FObjectInitializer& ObjectInitializer)
{
	//SetReplicates(true);
	bReplicates = true;
	
	//About GameReady property
	maxCReadyCount = 2;
	isReady = false;

	//Delete wrong playercontroller
	int32 indexCount = -1;

	if (GetWorld())
	{
		for (FConstPlayerControllerIterator iterator = GetWorld()->GetPlayerControllerIterator(); iterator; ++iterator)
		{
			++indexCount;
		}

		for (; indexCount > 0; --indexCount)
		{
			if (Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), indexCount)) == nullptr)
			{
				UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), indexCount), true);
			}
		}
	}
}

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//**Local
	//client only
	if (!HasAuthority())
	{
		FInputModeGameAndUI tempInputModeGameAndUI;
		tempInputModeGameAndUI.SetHideCursorDuringCapture(false);
		tempInputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(tempInputModeGameAndUI);

		bShowMouseCursor = true;

		cursorMode = ECursorMode::None;
	}

	//Async Asset Load
	/*
	if (!HasAuthority())
	{
		FStreamableManager& assetLoader = UAssetManager::GetStreamableManager();

		TArray<FSoftObjectPath> objectPath;
		UAssetManager::Get().GetPrimaryAssetPathList(FPrimaryAssetType(FName(TEXT("CharacterBase"))), objectPath);

		assetCharacter = assetLoader.RequestAsyncLoad(objectPath, FStreamableDelegate::CreateUObject(this, &AGamePlayerController::AssetLoadComplete));
	}*/
	

	if (!HasAuthority())
	{
		afterReadyDelegate.AddUniqueDynamic(this, &AGamePlayerController::AfterConnect);
	}

	ObjectReady(this);
}

void AGamePlayerController::AssetLoadComplete()
{
	UE_LOG(LogTemp, Display, TEXT("AssetLoadComplete!"));

	ObjectReady(this);
}

void AGamePlayerController::AfterConnect()
{
	//**Server Communication
	if (!HasAuthority())
	{
		Server_SetPlayerPosition();
		Server_FakePostLogin();

		isReady = true;

		if (PlayerState != nullptr && !GetPlayerHUD()->IsExecuted())
		{
			GetPlayerHUD()->SetMainUI();
		}
	}
}

AGameHUD* AGamePlayerController::GetPlayerHUD()
{
	if (playerHUD == nullptr)
	{
		playerHUD = Cast<AGameHUD>(GetHUD());
	}

	return playerHUD;
}

void AGamePlayerController::Client_EndGame_Implementation()
{
	//게임 종료.
	//로비로 접속.
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("175.212.195.101:7777"));
}

void AGamePlayerController::Server_FakePostLogin_Implementation()
{
	Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->PostLogin(this);
}

void AGamePlayerController::Server_SetPlayerPosition_Implementation()
{
	Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->SetPlayerStart(this);
}

void AGamePlayerController::OnRep_PlayerState()
{
	if (!HasAuthority())
	{
		if (!GetPlayerHUD()->IsExecuted())
		{
			GetPlayerHUD()->SetMainUI();
		}
	}
}

void AGamePlayerController::Client_afterSetPositionDelegate_Implementation()
{
	afterSetPositionDelegate.Broadcast();
}

void AGamePlayerController::afterSetPositionDeleBroad()
{
	Client_afterSetPositionDelegate();
}


void AGamePlayerController::ObjectReady(UObject* _object)
{
	if (!HasAuthority())
	{
		ReadyContainer.AddUnique(_object);

		if (ReadyContainer.Num() >= maxCReadyCount)
		{
			Server_ClientReady();
		}
	}
}

void AGamePlayerController::CallAfterReadyforServer()
{
	Client_CallAfterReady();
}

void AGamePlayerController::Server_ClientReady_Implementation()
{
	Cast<AToChessGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->ClientReady(this);
}

void AGamePlayerController::Client_CallAfterReady_Implementation()
{
	afterReadyDelegate.Broadcast();
}





/*
bool AGamePlayerController::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	if (characterManager) 
	{
		wrote |= Channel->ReplicateSubobject(characterManager, *Bunch, *RepFlags);
	}
	return wrote;
}*/
/*
void AGamePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGamePlayerController, tileCluster);
	DOREPLIFETIME(AGamePlayerController, characterManager);
}

void AGamePlayerController::SetCluster(ATileCluster* _cluster)
{
	if (_cluster != nullptr)
	{
		tileCluster = _cluster;
		_cluster->ownerPlayer = this;
	}
}

ATileCluster* AGamePlayerController::GetTileCluster()
{
	return tileCluster;
}

UCharacterManager* AGamePlayerController::GetCharacterManager()
{
	return characterManager;
}


void AGamePlayerController::SpawnCharacter_Implementation(int32 _num)
{
	if (!GetCharacterManager()->ExcuteUpgrade(_num))
	{
		GetCharacterManager()->SpawnCharacter(_num, EDeployMode::WaitingSpot, 1);
	}
}*/

