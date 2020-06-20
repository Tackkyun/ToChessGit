// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/DeployObject/TileCluster.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString
#include "Runtime/Engine/Classes/Components/ChildActorComponent.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ToChess/Public/Chracter/CharacterBase.h"
#include "ToChess/Private/GameMode/GamePlayStateBase.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include <queue>
#include "UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATileCluster::ATileCluster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Stationary;

	parentTile = CreateDefaultSubobject<USceneComponent>(TEXT("ParentTile"));
	parentTile->SetupAttachment(RootComponent);
	parentTile->Mobility = EComponentMobility::Stationary;

	friendlyParentSpot = CreateDefaultSubobject<USceneComponent>(TEXT("FriendlyParentSpot"));
	friendlyParentSpot->SetupAttachment(RootComponent);
	friendlyParentSpot->Mobility = EComponentMobility::Stationary;

	enemyParentSpot = CreateDefaultSubobject<USceneComponent>(TEXT("EnemyParentSpot"));
	enemyParentSpot->SetupAttachment(RootComponent);
	enemyParentSpot->Mobility = EComponentMobility::Stationary;

	boardSize = 8;
	tileDistance = 200.0f;

	waitingSpotSize = 8;
	spotDistance = 200.0f;
	basePosition = FVector::ZeroVector;

	ownerPlayer = nullptr;
	enemyPlayer = nullptr;

	locationCorrection = 0.0f;
}

void ATileCluster::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (tileArray.Num() > 0 && tileArray[0].tile[0] == nullptr)
	{
		RebuildFromSettings();
	}

	//Creating and Setting Tiles.
	/*
	if (applyButton)
	{
		if (GIsEditor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Delete and Create Tile!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("not Create Tile!"));
		}
	}*/
	
}

bool ATileCluster::IsConstructorChanged()
{
	if (parentTile->GetNumChildrenComponents() != boardSize * boardSize)
	{
		return true;
	}
	if (friendlyParentSpot->GetNumChildrenComponents() != waitingSpotSize)
	{
		return true;
	}

	return false;
}

void ATileCluster::CreatingAndSettingTiles()
{
		//if (boardSize == tileArray.Num()) return;
	if (parentTile == nullptr) return;

	//**Delete for Init
	while (parentTile->GetChildComponent(0) != nullptr)
	{
		parentTile->GetChildComponent(0)->DestroyComponent();
	}

	//**Set Vertex Position.
	float VertexPos = (boardSize - 1) / 2.0f * tileDistance; //중심으로 부터 가장 먼 곳.

	//**Set Tile Class
	TArray<UClass*> tempBPClass;

	for (int i = 0; i < BlueprintPath.Num(); ++i)
	{
		if (BlueprintPath[i].Len() > 0)
		{
			//Load BlueprintClass.
			tempBPClass.Add(LoadObject<UClass>(this, *BlueprintPath[i]));
			if (tempBPClass[i] == nullptr)
			{
				tempBPClass[i] = ATileObject::StaticClass();
			}
		}
		else
		{
			//Load DefaultClass
			tempBPClass.Add(ATileObject::StaticClass());
		}
	}

	if (BlueprintPath.Num() <= 0)
	{
		//Load DefaultClass
		tempBPClass.Add(ATileObject::StaticClass());
	}

	tileArray.Empty();

	//**Set tileArray
	for (int32 i = 0; i < boardSize; ++i)
	{
		tileArray.Add(FTileWidth());
		for (int32 j = 0; j < boardSize; ++j)
		{
			tileArray[i].tile.Add(nullptr);
		}
	}

	if (tempBPClass.Num() <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Error from TileCluster.cpp. tempBPClass.Num() is less than 1"));
		return;
	}

	int32 bluePrintClassNum = 0;

	//**Set Tiles on Cluster.
	for (int i = 0, checkPattern = 0; i < boardSize; ++i, checkPattern = i % tempBPClass.Num())//add Y
	{
		for (int j = 0; j < boardSize; ++j)//add X
		{
			//Create and Set Component.
			UChildActorComponent* tempCom = NewObject<UChildActorComponent>(this);

			bluePrintClassNum = ((j + checkPattern) % tempBPClass.Num());

			tempCom->SetChildActorClass(tempBPClass[bluePrintClassNum]);
			tempCom->RegisterComponent();

			FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
			tempCom->AttachToComponent(parentTile, rules);
			tempCom->ResetRelativeTransform();

			tempCom->SetRelativeLocation(
				( tempCom->GetRelativeRotation().Vector().LeftVector * (-VertexPos + (j * tileDistance)) ) +
				( tempCom->GetRelativeRotation().Vector().ForwardVector * (-VertexPos + (i * tileDistance)) )
			);
			tempCom->AddLocalRotation(FRotator(0.0f, 90.0f, 0.0f));

			ATileObject* tempObject = Cast<ATileObject>(tempCom->GetChildActor());
			tempObject->SetCoord(j, i);
			tempObject->SetOwnerCluster(this);
			if (i < (boardSize / 2))
			{
				tempObject->SetOwnerDeploy(true);
			}
			else
			{
				tempObject->SetOwnerDeploy(false);
			}

			tileArray[j].tile[i] = tempObject;
		}
	}
}

void ATileCluster::CreatingAndSettingSpots()
{
	//**Delete for Init
	while (friendlyParentSpot->GetChildComponent(0) != nullptr)
	{
		friendlyParentSpot->GetChildComponent(0)->DestroyComponent();
	}
	while (enemyParentSpot->GetChildComponent(0) != nullptr)
	{
		enemyParentSpot->GetChildComponent(0)->DestroyComponent();
	}
	friendlyWaitingSpotArray.Empty();
	enemyWaitingSpotArray.Empty();

	//**Setting BasePosition
	float leftVertexDist = (boardSize - 1) / 2.0f * tileDistance;
	float forwardVertexDist = (boardSize + 1) / 2.0f * tileDistance;
	FVector VertexPos = ( friendlyParentSpot->GetRelativeRotation().Vector().LeftVector * (-((waitingSpotSize - 1) / 2.0f * tileDistance)) ) + 
		( friendlyParentSpot->GetRelativeRotation().Vector().ForwardVector * (-((boardSize + 2) * tileDistance / 2.0f)) );

	UClass* tempClass;

	if (spotBlueprintPath.Len() > 0)
	{
		//Load BlueprintClass.
		tempClass = LoadObject<UClass>(this, *spotBlueprintPath);
		if (tempClass == nullptr)
		{
			tempClass = ACharacterWatingSpot::StaticClass();
		}
	}
	else
	{
		//Load DefaultClass
		tempClass = ACharacterWatingSpot::StaticClass();
	}

	
	//**Create Objects
	for (int i = 0; i < waitingSpotSize; ++i)
	{
		UChildActorComponent* tempCom = NewObject<UChildActorComponent>(this);

		tempCom->SetChildActorClass(tempClass);
		tempCom->RegisterComponent();

		FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
		tempCom->AttachToComponent(friendlyParentSpot, rules);
		tempCom->ResetRelativeTransform();
		
		tempCom->SetRelativeLocation(
			(friendlyParentSpot->GetRelativeRotation().Vector().LeftVector * (-leftVertexDist + (i * spotDistance))) +
			(friendlyParentSpot->GetRelativeRotation().Vector().ForwardVector * (-forwardVertexDist))
		);
		tempCom->AddLocalRotation(FRotator(0.0f, 90.0f, 0.0f));

		Cast<ATileBase>(tempCom->GetChildActor())->SetCoord(i, 0);
		Cast<ATileBase>(tempCom->GetChildActor())->SetOwnerCluster(this);
		Cast<ATileBase>(tempCom->GetChildActor())->SetOwnerDeploy(true);

		friendlyWaitingSpotArray.Add(Cast<ACharacterWatingSpot>(tempCom->GetChildActor()));
	}

	VertexPos = ( enemyParentSpot->GetRelativeRotation().Vector().RightVector * ((waitingSpotSize - 1) / 2.0f * tileDistance) ) +
		( enemyParentSpot->GetRelativeRotation().Vector().ForwardVector * ((boardSize + 2) * tileDistance / 2.0f) );

	for (int i = 0; i < waitingSpotSize; ++i)
	{
		UChildActorComponent* tempCom = NewObject<UChildActorComponent>(this);

		tempCom->SetChildActorClass(tempClass);
		tempCom->RegisterComponent();

		FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
		tempCom->AttachToComponent(enemyParentSpot, rules);
		tempCom->ResetRelativeTransform();

		tempCom->SetRelativeLocation(
			(enemyParentSpot->GetRelativeRotation().Vector().RightVector * (leftVertexDist - (i * spotDistance))) +
			(enemyParentSpot->GetRelativeRotation().Vector().ForwardVector * (forwardVertexDist))
		);
		tempCom->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));

		Cast<ATileBase>(tempCom->GetChildActor())->SetCoord(i, 0);
		Cast<ATileBase>(tempCom->GetChildActor())->SetOwnerCluster(this);
		Cast<ATileBase>(tempCom->GetChildActor())->SetOwnerDeploy(false);

		enemyWaitingSpotArray.Add(Cast<ACharacterWatingSpot>(tempCom->GetChildActor()));
	}
}

void ATileCluster::RebuildFromSettings()
{
	CreatingAndSettingTiles();
	CreatingAndSettingSpots();
}

void ATileCluster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATileCluster, friendlyParentSpot);
	DOREPLIFETIME(ATileCluster, enemyParentSpot);
	DOREPLIFETIME(ATileCluster, friendlyWaitingSpotArray);
	DOREPLIFETIME(ATileCluster, enemyWaitingSpotArray);

	DOREPLIFETIME(ATileCluster, AIPlayerCharacter);
	DOREPLIFETIME(ATileCluster, ownerPlayer);
	DOREPLIFETIME(ATileCluster, enemyPlayer);
	DOREPLIFETIME(ATileCluster, recordedCharacters);
}

// Called when the game starts or when spawned
void ATileCluster::BeginPlay()
{
	Super::BeginPlay();
	
	if (tileArray.Num() > 0 && tileArray[0].tile[0] == nullptr)
	{
		SetTileArray();
	}
	else if (tileArray.Num() == 0)
	{
		RebuildFromSettings();
	}

	SetNearTile();
	SetSpotArray();
	

	//bind
	if (HasAuthority())
	{
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->enterDelegate.AddUniqueDynamic(this, &ATileCluster::RunRecord);
		Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &ATileCluster::RecoverFromRecord);
	}

	//SetAI
	if (HasAuthority())
	{
		FActorSpawnParameters tempParam = FActorSpawnParameters();
		tempParam.Owner = this;
		AIPlayerCharacter = GetWorld()->SpawnActor<AAIGamePlayerCharacter>(AAIGamePlayerCharacter::StaticClass(), tempParam);
		AIPlayerCharacter->SetOwner(this);
		AIPlayerCharacter->SetCluster(this);
	}
}

void ATileCluster::BindGameState()
{
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Ready")->enterDelegate.AddUniqueDynamic(this, &ATileCluster::RunRecord);
	Cast<AMainGameState>(GetWorld()->GetGameState())->GetStateInContainer("Exit")->exitDelegate.AddUniqueDynamic(this, &ATileCluster::RecoverFromRecord);
}

void ATileCluster::SetTileArray()
{
	tileArray.Empty();

	{
		FVector correctionDist = (parentTile->GetRightVector() * -(tileDistance * (boardSize - 1) / 2.0f)) + (parentTile->GetForwardVector() * (-(tileDistance * (boardSize - 1) / 2.0f)));
		FTransform baseTransform = FTransform(parentTile->GetRelativeRotation(), 
			parentTile->GetRelativeLocation() + correctionDist, 
			parentTile->GetRelativeScale3D());
		baseTransform = baseTransform * GetTransform();

		for (int32 i = 0; i < boardSize; ++i)
		{
			tileArray.Add(FTileWidth());
			for (int32 j = 0; j < boardSize; ++j)
			{
				tileArray[i].tile.Add(nullptr);
			}
		}

		for (int32 i = 0; i < parentTile->GetNumChildrenComponents(); ++i)
		{
			AActor* chooseActor = Cast<UChildActorComponent>(parentTile->GetChildComponent(i))->GetChildActor();
			FTransform tempTM = chooseActor->GetActorTransform();
			FTransform localTransform = UKismetMathLibrary::MakeRelativeTransform(baseTransform, tempTM);
			int32 y = FMath::Abs<int32>(FMath::RoundToInt(localTransform.GetLocation().Y / tileDistance));
			int32 x = FMath::Abs<int32>(FMath::RoundToInt(localTransform.GetLocation().X / tileDistance));

			if (Cast<ATileObject>(chooseActor))
			{
				tileArray[x].tile[y] = Cast<ATileObject>(chooseActor);
				Cast<ATileBase>(chooseActor)->SetCoord(x, y);
				Cast<ATileBase>(chooseActor)->SetOwnerCluster(this);
				if (y < (boardSize / 2))
				{
					Cast<ATileBase>(chooseActor)->SetOwnerDeploy(true);
				}
				else
				{
					Cast<ATileBase>(chooseActor)->SetOwnerDeploy(false);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("TileCluster.cpp SetTileArray() error"));
			}
		}
	}
}

void ATileCluster::SetNearTile()
{
	if (tileArray.Num() == 0)
	{
		return;
	}

	//**Make Graph
	for (int32 i = 0; i < boardSize; ++i)
	{
		for (int32 j = 0; j < boardSize; ++j)
		{
			tileArray[i].tile[j]->SetOwnerCluster(this);
			if (j < (boardSize / 2))
			{
				tileArray[i].tile[j]->SetOwnerDeploy(true);
			}
			else
			{
				tileArray[i].tile[j]->SetOwnerDeploy(false);
			}

			//Find near
			for (int y = -1; y <= 1; ++y)
			{
				for (int x = -1; x <= 1; ++x)
				{
					if (y == 0 && x == 0)
					{
						continue;
					}

					int tempX = i + x;
					int tempY = j + y;

					if (tempX >= 0 && tempX < boardSize &&
						tempY >= 0 && tempY < boardSize)
					{
						tileArray[i].tile[j]->nearTile.Add(tileArray[tempX].tile[tempY]);
					}
				}
			}	
		}
	}
}

void ATileCluster::SetSpotArray()
{
	friendlyWaitingSpotArray.Empty();
	enemyWaitingSpotArray.Empty();

	if (friendlyParentSpot->GetNumChildrenComponents() > 1)
	{
		FVector correctionDist = friendlyParentSpot->GetRelativeRotation().Vector().LeftVector * ((waitingSpotSize - 1) * spotDistance / 2.0f);
		FTransform baseTransform = FTransform(friendlyParentSpot->GetRelativeRotation(),
			friendlyParentSpot->GetRelativeLocation() + correctionDist,
			friendlyParentSpot->GetRelativeScale3D());
		baseTransform = baseTransform * GetTransform();

		for (int32 i = 0; i < waitingSpotSize; ++i)
		{
			friendlyWaitingSpotArray.Add(nullptr);
			enemyWaitingSpotArray.Add(nullptr);
		}

		for (int i = 0; i < waitingSpotSize; ++i)
		{
			AActor* chooseActor = Cast<UChildActorComponent>(friendlyParentSpot->GetChildComponent(i))->GetChildActor();
			FTransform tempTM = chooseActor->GetActorTransform();
			FTransform localTransform = UKismetMathLibrary::MakeRelativeTransform(baseTransform, tempTM);

			int32 x = FMath::Abs(FMath::RoundToInt(localTransform.GetLocation().X / spotDistance));

			Cast<ATileBase>(chooseActor)->SetCoord(x, 0);
			Cast<ATileBase>(chooseActor)->SetOwnerCluster(this);
			Cast<ATileBase>(chooseActor)->SetOwnerDeploy(true);

			Cast<ATileBase>(chooseActor)->SetOwnerCluster(this);
			Cast<ATileBase>(chooseActor)->SetOwnerDeploy(true);

			friendlyWaitingSpotArray[x] = Cast<ACharacterWatingSpot>(chooseActor);
		}

		correctionDist = enemyParentSpot->GetRelativeRotation().Vector().RightVector * ((waitingSpotSize - 1) * spotDistance / 2.0f);
		baseTransform = FTransform(enemyParentSpot->GetRelativeRotation(),
			enemyParentSpot->GetRelativeLocation() + correctionDist,
			enemyParentSpot->GetRelativeScale3D());
		baseTransform = baseTransform * GetTransform();

		for (int i = 0; i < waitingSpotSize; ++i)
		{
			AActor* chooseActor = Cast<UChildActorComponent>(enemyParentSpot->GetChildComponent(i))->GetChildActor();
			FTransform tempTM = chooseActor->GetActorTransform();
			FTransform localTransform = UKismetMathLibrary::MakeRelativeTransform(baseTransform, tempTM);

			int32 x = FMath::Abs(FMath::RoundToInt(localTransform.GetLocation().X / spotDistance));

			Cast<ATileBase>(chooseActor)->SetCoord(x, 0);
			Cast<ATileBase>(chooseActor)->SetOwnerCluster(this);
			Cast<ATileBase>(chooseActor)->SetOwnerDeploy(false);

			Cast<ATileBase>(chooseActor)->SetOwnerCluster(this);
			Cast<ATileBase>(chooseActor)->SetOwnerDeploy(false);

			enemyWaitingSpotArray[x] = Cast<ACharacterWatingSpot>(chooseActor);
		}
	}

	
}

// Called every frame
void ATileCluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//return A* route
TArray<ATileObject*> ATileCluster::FindRoute(ATileObject* _start, ATileObject* _target)
{
	TArray<ATileObject*> result;

	if (_target == nullptr || _start == nullptr)
	{
		return result;
	}
	
	//방문된 노드 정보.
	TMap<ATileObject*, FTileNode> tileInfo;

	//우선순위 큐.
	std::priority_queue<FTileNode, std::vector<FTileNode>, std::greater<FTileNode>> pq;

	FTileNode beginNode(_start, FVector::Dist2D(_start->GetActorLocation(), _target->GetActorLocation()), 0.0f);
	
	pq.push(beginNode);
	tileInfo.Add(_start, beginNode);

	//edge
	TMultiMap<ATileObject*, ATileObject*> visitedEdge;

	//탐색중인 현 노드위치.
	FTileNode currentNode(pq.top().tile, pq.top().hCost, pq.top().cost);

	while (!pq.empty())
	{
		currentNode = pq.top();
		pq.pop();

		//종료조건. 목적지라면
		if (currentNode.tile == _target) break;

		//인근 노드 탐색.
		TArray<ATileObject*> currentNearTiles = currentNode.tile->nearTile;
		while (currentNearTiles.Num() > 0)
		{
			//예외. 방문한 적 있다면
			if (visitedEdge.FindPair(currentNode.tile, currentNearTiles.Top()) != nullptr ||
				visitedEdge.FindPair(currentNearTiles.Top(), currentNode.tile) != nullptr)
			{
				currentNearTiles.Pop();
				continue;
			}
			
			//예외. 타일 주인이 존재한다면.
			if (currentNearTiles.Top()->characterSlot != nullptr)
			{
				if (currentNearTiles.Top() != _target &&
					currentNearTiles.Top() != _start)
				{
					currentNearTiles.Pop();
					continue;
				}
			}

			//visited edge 기록.
			visitedEdge.Add(currentNode.tile, currentNearTiles.Top());

			//방문노드.
			FTileNode tempNode(currentNearTiles.Top(),
				FVector::Dist2D(currentNearTiles.Top()->GetActorLocation(), _target->GetActorLocation()),
				currentNode.cost + tileDistance);

			//최종 노드는 대각선으로 마무리 하지 않음. 십자모양으로만 최종 노드 판별.
			if (currentNearTiles.Top() == _target &&
				FVector::Dist2D(currentNearTiles.Top()->GetActorLocation(), currentNode.tile->GetActorLocation()) >= tileDistance * 1.1f)
			{
				currentNearTiles.Pop();
				continue;
			}
				//최단기록이 없다면.
			if (tempNode.preNode == nullptr)
			{
				tempNode.preNode = currentNode.tile;
				pq.push(tempNode);
			}
			//최단기록이 있지만, 새로운 경로가 더 빠를 경우.
			else if (tempNode.cost + tempNode.hCost > currentNode.cost + currentNode.hCost + tileDistance)
			{
				tempNode.preNode = currentNode.tile;
			}

			tileInfo.Add(currentNearTiles.Top(), tempNode);
			

			currentNearTiles.Pop();
		}
	}

	//탐색 실패.
	if (currentNode.tile != _target)
	{
		return result;
	}

	//최단루트 기록.
	while (currentNode.tile != _start)
	{
		result.Add(currentNode.preNode);
		currentNode = *tileInfo.Find(currentNode.preNode);
	}
	
	return result;
}

//Find Enemy
ACharacterBase* ATileCluster::FindTarget(FVector _pos, int _range, ACharacterBase* _self)
{
	APlayerCharacterBase* tempOtherPlayer = GetOtherPlayer(_self->GetOwnerPawn());
	TArray<ACharacterBase*> tempData = Cast<APlayerCharacterBase>(tempOtherPlayer)->GetCharacterManager()->GetCharactersInTiles();
	
	ACharacterBase* returnData = nullptr;

	for (int i = 0; i < tempData.Num(); ++i)
	{
		//예외사항. 적 여부 추가할것.
		if (tempData[i]->status->hp <= 0 ||
			_self == tempData[i] ||
			tempData[i] == nullptr ||
			_self->GetOwnerPawn() == tempData[i]->GetOwnerPawn()) continue;

		if (FVector::Dist2D(_pos, tempData[i]->GetActorLocation()) <= (_range * tileDistance + (tileDistance * 0.1f)) && Cast<ATileBase>(tempData[i]->hasTile))
		{
			if (returnData == nullptr)
			{
				returnData = tempData[i];
			}
			else if(FVector::Dist2D(_pos, tempData[i]->GetActorLocation()) <= FVector::Dist2D(_pos, returnData->GetActorLocation()))
			{
				returnData = tempData[i];
			}
		}
	}

	return returnData;
}

int32 ATileCluster::GetEnimiesNumInField(APlayerCharacterBase* _self)
{
	if (enemyPlayer >= 0)
	{
		return Cast<APlayerCharacterBase>(GetOtherPlayer(_self))->GetCharacterManager()->GetCharactersInTiles().Num();
	}
	else
	{
		return 0;
	}
}

int32 ATileCluster::GetFriendlyNumInField(APlayerCharacterBase* _self)
{
	return Cast<APlayerCharacterBase>(_self)->GetCharacterManager()->GetCharactersInTiles().Num();
}

bool ATileCluster::DeployFriendlyWaitingSpot(ACharacterBase* _character)
{
	if (_character == nullptr) return false;

	for (int i = 0; i < friendlyWaitingSpotArray.Num(); ++i) 
	{
		if (friendlyWaitingSpotArray[i]->characterSlot == nullptr)
		{
			friendlyWaitingSpotArray[i]->characterSlot = (_character);
			_character->SetActorTransform(friendlyWaitingSpotArray[i]->GetSpawnPosition());
			_character->hasTile = friendlyWaitingSpotArray[i];
			return true;
		}
	}

	return false;
}

bool ATileCluster::DeployFriendlyWaitingSpotAt(ACharacterBase* _character, int32 _x)
{
	if (_character == nullptr || _x >= waitingSpotSize)
	{
		return false;
	}

	friendlyWaitingSpotArray[_x]->characterSlot = _character;
	_character->SetActorTransform(friendlyWaitingSpotArray[_x]->GetSpawnPosition());
	_character->hasTile = friendlyWaitingSpotArray[_x];

	return true;
}

bool ATileCluster::DeployFriendlyTile(ACharacterBase* _character)
{
	if (_character == nullptr) return false;

	for (int i = 0; i < boardSize; ++i)
	{
		for (int j = 0; j < boardSize; ++j)
		{
			if (tileArray[i].tile[j]->characterSlot == nullptr)
			{
				tileArray[i].tile[j]->characterSlot = (_character);
				_character->SetActorTransform(tileArray[i].tile[j]->GetSpawnPosition());
				_character->hasTile = tileArray[i].tile[j];
				return true;
			}
		}
	}

	return false;
}

bool ATileCluster::DeployCharacterToTile(ACharacterBase* _character, int32 _x, int32 _y)
{
	if (_character == nullptr ||
		_x >= boardSize || _y >= boardSize ||
		tileArray[_x].tile[_y]->characterSlot != nullptr)
	{
		return false;
	}

	tileArray[_x].tile[_y]->characterSlot = (_character);
	_character->SetActorTransform(tileArray[_x].tile[_y]->GetSpawnPosition());
	_character->hasTile = tileArray[_x].tile[_y];

	return true;
}

bool ATileCluster::DeployEnemyWaitingSpot(ACharacterBase* _character)
{
	if (_character == nullptr) return false;

	for (int i = 0; i < enemyWaitingSpotArray.Num(); ++i)
	{
		if (enemyWaitingSpotArray[i]->characterSlot == nullptr)
		{
			enemyWaitingSpotArray[i]->characterSlot = (_character);
			_character->SetActorTransform(enemyWaitingSpotArray[i]->GetSpawnPosition());
			_character->hasTile = enemyWaitingSpotArray[i];
			return true;
		}
	}

	return false;
}

bool ATileCluster::DeployEnemyWaitingSpotAt(ACharacterBase* _character, int32 _x)
{
	if (_character == nullptr || _x >= waitingSpotSize)
	{
		return false;
	}

	enemyWaitingSpotArray[_x]->characterSlot = _character;
	_character->SetActorTransform(enemyWaitingSpotArray[_x]->GetSpawnPosition());
	_character->hasTile = enemyWaitingSpotArray[_x];

	return true;
}

bool ATileCluster::IsWaitingSpotFull()
{
	for (int i = 0; i < friendlyWaitingSpotArray.Num(); ++i)
	{
		if (friendlyWaitingSpotArray[i]->characterSlot != nullptr)
		{
			return true;
		}
	}

	return false;
}

bool ATileCluster::IsWaitingSpotFullAuto(APlayerCharacterBase* _ownerPawn)
{
	if (ownerPlayer == _ownerPawn)
	{
		for (int i = 0; i < friendlyWaitingSpotArray.Num(); ++i)
		{
			if (friendlyWaitingSpotArray[i]->characterSlot != nullptr)
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < enemyWaitingSpotArray.Num(); ++i)
		{
			if (enemyWaitingSpotArray[i]->characterSlot != nullptr)
			{
				return true;
			}
		}
	}

	return false;
}

APlayerCharacterBase* ATileCluster::GetOwnerPlayer()
{
	return ownerPlayer;
}

void ATileCluster::SetOwnerPlayer(APlayerCharacterBase* _player)
{
	Multi_SetOwnerPlayer(_player);
}

void ATileCluster::Multi_SetOwnerPlayer_Implementation(APlayerCharacterBase* _player)
{
	ownerPlayer = _player;
}

APlayerCharacterBase* ATileCluster::GetOtherPlayer(APlayerCharacterBase* _self)
{
	if (_self == ownerPlayer)
	{
		if (enemyPlayer == nullptr)
		{
			return AIPlayerCharacter;
		}
		else
		{
			return enemyPlayer;
		}
	}
	else
	{
		return ownerPlayer;
	}

}

FTransform ATileCluster::GetPlayerPosition(bool _isEnemy)
{
	FTransform result;

	if (!_isEnemy)
	{
		result = FTransform(GetActorRotation(),
			(friendlyWaitingSpotArray[0]->GetActorLocation() + friendlyWaitingSpotArray[waitingSpotSize - 1]->GetActorLocation()) / 2);

		//Correction.
		result.SetLocation(result.GetLocation() +
			(result.GetRotation().Vector().ForwardVector * locationCorrection));
	}
	else
	{
		
		FRotator tempRot(GetActorRotation().Pitch, GetActorRotation().Yaw + 180.0f, GetActorRotation().Roll);

		FVector tempVec = (enemyWaitingSpotArray[0]->GetActorLocation() + enemyWaitingSpotArray[waitingSpotSize - 1]->GetActorLocation()) / 2;

		result = FTransform(tempRot, tempVec);

		//Correction.
		result.SetLocation(result.GetLocation() -
			(result.GetRotation().Vector().ForwardVector * locationCorrection));
	}

	

	return result;
}

void ATileCluster::RunRecord()
{
	recordedCharacters.Empty();

	for (int32 i = 0; i < boardSize; ++i)
	{
		for (int32 j = 0; j < boardSize; ++j)
		{
			if (tileArray[i].tile[j]->characterSlot != nullptr && tileArray[i].tile[j]->characterSlot->GetOwnerPawn() == GetOwnerPlayer())
			{
				FCharacterRecordElement tempElement;
				tempElement.x = j;
				tempElement.y = i;
				tempElement.character = tileArray[i].tile[j]->characterSlot;
				recordedCharacters.Add(tempElement);
			}
		}
	}
}

void ATileCluster::RecoverFromRecord()
{
	Cast<APlayerCharacterBase>(ownerPlayer)->GetCharacterManager()->RemoveAllCharacterInTile();
	Cast<APlayerCharacterBase>(GetOtherPlayer(ownerPlayer))->GetCharacterManager()->DestroyAllCharacterInTile();
	

	for (int32 i = 0; i < recordedCharacters.Num(); ++i)
	{
		Cast<ACharacterBase>(recordedCharacters[i].character)->Server_SwapTile(tileArray[recordedCharacters[i].y].tile[recordedCharacters[i].x]);
	}

	for (int32 i = 0; i < recordedCharacters.Num(); ++i)
	{
		ownerPlayer->GetCharacterManager()->AddInTile(Cast<ACharacterBase>(recordedCharacters[i].character));
		Cast<ACharacterBase>(recordedCharacters[i].character)->ResetCharacter();
	}
}

AAIGamePlayerCharacter* ATileCluster::GetAIPlayerCharacter()
{
	return AIPlayerCharacter;
}