// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/Chracter/CharacterBase.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ToChess/Public/DeployObject/TileBase.h"
#include "ToChess/Public/DeployObject/CharacterWatingSpot.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Animation/AnimBlueprintGeneratedClass.h"
#include "ToChess/Public/DeployObject/TileObject.h"
#include "ToChess/Public/Chracter/CharacterManager.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"
#include "Runtime/Engine/Classes/Engine/Engine.h" //PrintString
#include "ToChess/Public/GameMode/MainGameState.h"
#include "ToChess/Public/GameMode/GamePlayerState.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "ToChess/Private/DataTableBase/DataTableManager.h"
#include "ToChess/Public/GameMode/PlayerCharacterBase.h"
#include "UnrealNetwork.h"
#include "ToChess/Public/FunctionClass/MainGameFunctionLibrary.h"
#include "Animation/AnimInstance.h"
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("cTick"));

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	UE_LOG(LogTemp, Log, TEXT("Character Init Start"));
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	status = CreateDefaultSubobject<UCharacterStatus>(TEXT("Status"));
	
	//AIControllerClass = ACharacterAIconBase::StaticClass();

	isFollowMouse = false;

	hpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	hpBarWidget->SetupAttachment(RootComponent);
	hpBarWidget->SetDrawAtDesiredSize(false);

	isDestroyed = false;

	SetOwnerPawn(Cast<APlayerCharacterBase>(GetOwner()));

	UE_LOG(LogTemp, Log, TEXT("Character Init End"));
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, status);
	DOREPLIFETIME(ACharacterBase, ownerPawn);
	DOREPLIFETIME(ACharacterBase, hasTile);
	DOREPLIFETIME(ACharacterBase, detectTile);
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

APlayerCharacterBase* ACharacterBase::GetOwnerPawn()
{
	return ownerPawn;
}

void ACharacterBase::SetOwnerPawn_Implementation(APlayerCharacterBase* _owner)
{
	ownerPawn = _owner;
}

void ACharacterBase::SetStatusData(FDTCharacterStatus _statusData, int32 _star)
{
	statusData = _statusData;
	status->InitData(&statusData, _star);
	
	GetMesh()->SetRelativeScale3D(GetMesh()->GetRelativeTransform().GetScale3D() + (FVector::OneVector * _star * 0.1f));
}

void ACharacterBase::SetMeshMaterial_Implementation(int32 _index, UMaterialInterface* _material)
{
	GetMesh()->SetMaterial(_index, _material);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Character BeginPlay Start"));

	Super::BeginPlay();

	if (!HasAuthority())
	{
		GetCapsuleComponent()->OnInputTouchBegin.AddUniqueDynamic(this, &ACharacterBase::ConnectFollowMouse);
	}

	if (Cast<APlayerCharacterBase>(GetOwner()))
	{
		SetOwnerPawn(Cast<APlayerCharacterBase>(GetOwner()));
	}
	
	if (HasAuthority())
	{
		Multi_SetAIController();
	}

	UE_LOG(LogTemp, Log, TEXT("Character BeginPlay End"));
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Resizing HPWidget to Camera. delegate 호출코드 아래 두면 destroy 호출 시 에러.
	
	if (!HasAuthority())
	{
		float sizeValue = UKismetMathLibrary::MapRangeClamped(
			UKismetMathLibrary::VSize(
				hpBarWidget->GetComponentLocation() - UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation()
			),
			0.0f, 2000.0f, 1.0f, 0.0f);

		hpBarWidget->GetUserWidgetObject()->SetRenderScale(UKismetMathLibrary::MakeVector2D(sizeValue, sizeValue));
	}
	
	//excute Delegate
	if (m_tickDelegate.IsBound())
	{
		m_tickDelegate.Execute();
	}
	
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}


float ACharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//apply damage
	status->hp -= (DamageAmount * (DamageAmount / (DamageAmount + status->defensivePower)));

	if (status->hp <= 0)
	{
		DeathCharacter(EventInstigator);
	}

	return 0.0f;
}

void ACharacterBase::Multi_SetAIController_Implementation()
{
	SpawnDefaultController();
}

void ACharacterBase::Server_RunAttackAnimation_Implementation()
{
	Multi_RunAttackAnimation();
}

void ACharacterBase::Multi_RunAttackAnimation_Implementation()
{
	if (attackAnimMontage != nullptr)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(attackAnimMontage, status->attackSpeed);
	}
}

void ACharacterBase::DeathCharacter(AController* EventInstigator)
{
	//Unbind CharacterManager
	GetOwnerPawn()->GetCharacterManager()->RemoveInTile(this);

	HiddenCharacter();
	//Destroy();
	
}

void ACharacterBase::ResetCharacter()
{
	VisibleCharacter();
	status->ResetData();
}

void ACharacterBase::HiddenCharacter()
{
	SetActorHiddenInGame(true);
	hpBarWidget->SetVisibility(false);
	PrimaryActorTick.bCanEverTick = false;
	m_tickDelegate.Unbind();
}

void ACharacterBase::VisibleCharacter()
{
	SetActorHiddenInGame(false);
	hpBarWidget->SetVisibility(true);
	PrimaryActorTick.bCanEverTick = true;
}

void ACharacterBase::SellThis()
{
	FDTCharacterStatus* temp = UDataTableManager::GetInst()->GetDTCharacterStatus(status->star)->FindRow<FDTCharacterStatus>(FName(*FString::FromInt(status->num)), "");
	GetOwnerPawn()->GetController()->GetPlayerState<AGamePlayerState>()->Server_AddMoney(temp->Cost);

	Server_DestroyThis();
}

void ACharacterBase::Server_DestroyThis_Implementation()
{
	if (hasTile != nullptr)
	{
		hasTile->characterSlot = nullptr;

		if (Cast<ATileObject>(hasTile))
		{
			GetOwnerPawn()->GetCharacterManager()->RemoveInTile(this);
		}
		else
		{
			GetOwnerPawn()->GetCharacterManager()->RemoveInWaitingSpot(this);
		}

		hasTile = nullptr;
	}
	
	Destroy();
}

void ACharacterBase::MoveStraight(ATileBase* _target, float _interval)
{
	if (_target->characterSlot == nullptr || _target->characterSlot == this)
	{
		m_preTile = hasTile;
		m_preTile->characterSlot = nullptr;
		_target->characterSlot = this;
		hasTile = _target;
	}
	else
	{
		//stop and unbind
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		m_tickDelegate.Unbind();
		motionEndDelegate.Broadcast();
		motionEndDelegate.RemoveAll(this);
	}

	//Setting
	m_dir = FVector(_target->GetActorLocation().X, _target->GetActorLocation().Y, 0.0f) -
		FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f);
	m_interval = _interval;

	//Delegate unbound and bind
	if (m_tickDelegate.IsBound())
	{
		m_tickDelegate.Unbind();
	}

	if (motionEndDelegate.IsBound())
	{
		motionEndDelegate.RemoveAll(this);
	}

	m_tickDelegate.BindUObject(this, &ACharacterBase::Delegate_MoveStraight);
}

void ACharacterBase::Delegate_MoveStraight()
{
	//goal
	if (FVector::DistXY(hasTile->GetActorLocation(), GetActorLocation()) <= m_interval)
	{
		//stop and unbind
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		m_tickDelegate.Unbind();
		motionEndDelegate.Broadcast();
		motionEndDelegate.RemoveAll(this);

		return;
	}

	if (FVector::DistXY(m_preTile->GetActorLocation(), GetActorLocation()) < FVector::DistXY(hasTile->GetActorLocation(), m_preTile->GetActorLocation()) + m_interval)
	{
		if (FVector::DistXY(hasTile->GetActorLocation(), GetActorLocation()) > m_interval)
		{
			GetCharacterMovement()->MaxWalkSpeed = status->moveSpeed * 500.0f;
			SetActorRotation(FRotationMatrix::MakeFromX(m_dir).Rotator());
			AddMovementInput(m_dir, 1.0f);
		}
	}
	else
	{
		SetActorLocation(hasTile->GetSpawnPosition().GetLocation());
	}
}


//복잡함. 함수 나누고, 수정할것.
void ACharacterBase::Delegate_FollowMouse()
{
	//Exception. Start Gameset.
	if (GetWorld()->GetGameState<AMainGameState>()->GetCurrentState() != "WaitTime" && Cast<ATileObject>(hasTile))
	{
		SetActorTransform(hasTile->GetSpawnPosition());
		m_tickDelegate.Unbind();
		Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::None;
		return;
	}

	//Check Touched this
	if (UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0)->IsInputKeyDown(EKeys::TouchKeys[0]))
	{
		FHitResult hitResult;
		if (UMainGameFunctionLibrary::GetMainGamePlayerController(GetWorld(), 0)->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, hitResult) )
		{
			SetActorLocation(FVector(hitResult.Location.X, hitResult.Location.Y, hitResult.Location.Z + 90.0f));
			ATileBase* hitTile = Cast<ATileBase>(hitResult.GetActor());
			detectTile = hitTile;
		}
	}
	//if TouchEnd
	else
	{
		if (Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode == ECursorMode::SellCharacter)
		{
			Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::None;
			m_tickDelegate.Unbind();
			SellThis();
			return;
		}

		if (detectTile == nullptr || !detectTile->IsDeploy(GetOwnerPawn()))
		{
			//Position Return
			SetActorTransform(hasTile->GetSpawnPosition());

			Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::None;
			m_tickDelegate.Unbind();

			return;
		}

		if (detectTile != nullptr)
		{
			//Setting TileData in CharacterManager
			if (!Cast<ATileObject>(hasTile)) // SpotPoint -> FieldTile
			{
				if (Cast<ATileObject>(detectTile))
				{
					//예외처리
					if (GetWorld()->GetGameState<AMainGameState>()->GetCurrentState() != "WaitTime")
					{
						SetActorTransform(hasTile->GetSpawnPosition());
						m_tickDelegate.Unbind();
						Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::None;
						return;
					}

					if (detectTile->characterSlot != nullptr)
					{
						GetOwnerPawn()->GetCharacterManager()->RemoveInTile(detectTile->characterSlot);
					}

					//Add CharacterManager
					GetOwnerPawn()->GetCharacterManager()->AddInTile(this);
				}
			}
			else // FieldTile -> SpotPoint
			{
				if (!Cast<ATileObject>(detectTile))
				{
					//매니저에서 해제.
					GetOwnerPawn()->GetCharacterManager()->RemoveInTile(this);
				}
			}

			//Swap Tile
			Server_SwapTile(detectTile);
		}

		m_tickDelegate.Unbind();
		Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::None;
	}
}

void ACharacterBase::ConnectFollowMouse(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	if (Cast<ATileObject>(hasTile) && GetWorld()->GetGameState<AMainGameState>()->GetCurrentState() != "WaitTime")
	{
		return;
	}

	if (!m_tickDelegate.IsBound())
	{
		m_tickDelegate.BindUObject(this, &ACharacterBase::Delegate_FollowMouse);
		Cast<AGamePlayerController>(GetOwnerPawn()->GetController())->cursorMode = ECursorMode::CharacterFollow;
	}
}

void ACharacterBase::Server_SwapTile_Implementation(ATileBase* _target)
{
	Multi_SwapTile(_target);
}

void ACharacterBase::Multi_SwapTile_Implementation(ATileBase* _target)
{
	if (hasTile == nullptr)
	{
		hasTile = _target;
		SetActorTransform(hasTile->GetSpawnPosition());
		if (Cast<ATileObject>(hasTile))
		{
			GetOwnerPawn()->GetCharacterManager()->AddInTile(this);
		}
		else
		{
			GetOwnerPawn()->GetCharacterManager()->AddInWaitingSpot(this);
		}

		return;
	}

	Swap<ACharacterBase*>(hasTile->characterSlot, _target->characterSlot);
	Swap<ATileBase*>(hasTile, _target);
	SetActorTransform(hasTile->GetSpawnPosition());

	if (_target->characterSlot != nullptr)
	{
		_target->characterSlot->hasTile = _target;
		_target->characterSlot->SetActorTransform(_target->GetSpawnPosition());
	}

	//WaitingSpot to Field
	if (Cast<ATileObject>(hasTile) && Cast<ACharacterWatingSpot>(_target))
	{
		GetOwnerPawn()->GetCharacterManager()->AddInTile(this);
		GetOwnerPawn()->GetCharacterManager()->RemoveInWaitingSpot(this);

		if (_target->characterSlot != nullptr)
		{
			_target->characterSlot->GetOwnerPawn()->GetCharacterManager()->AddInWaitingSpot(_target->characterSlot);
			_target->characterSlot->GetOwnerPawn()->GetCharacterManager()->RemoveInTile(_target->characterSlot);
		}
	}
	//Field to WaitingSpot
	else if (Cast<ATileObject>(_target) && Cast<ACharacterWatingSpot>(hasTile))
	{
		GetOwnerPawn()->GetCharacterManager()->AddInWaitingSpot(this);
		GetOwnerPawn()->GetCharacterManager()->RemoveInTile(this);

		if (_target->characterSlot != nullptr)
		{
			_target->characterSlot->GetOwnerPawn()->GetCharacterManager()->AddInTile(_target->characterSlot);
			_target->characterSlot->GetOwnerPawn()->GetCharacterManager()->RemoveInWaitingSpot(_target->characterSlot);
		}
	}
}

FPrimaryAssetId ACharacterBase::GetPrimaryAssetId() const
{
	static const FPrimaryAssetType assetType = FName(TEXT("CharacterBase"));
	FString assetName = GetClass()->GetFName().ToString().LeftChop(2);
	
	return FPrimaryAssetId(assetType, FName(*assetName));
}