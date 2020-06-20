// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ToChess/Public/Chracter/CharacterStatus.h"
#include "ToChess/Public/Chracter/CharacterAIconBase.h"
#include "ToChess/Private/DataTableBase/DT_Character.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Animation/AnimMontage.h"
#include "CharacterBase.generated.h"

class ATileBase;
class APlayerCharacterBase;

DECLARE_DELEGATE(FCharacterTickDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterMotionEndDelegate);

UCLASS()
class TOCHESS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	virtual void OnConstruction(const FTransform& Transform);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(NetMultiCast, Reliable, BlueprintCallable, Category = "CharacterBase")
	void Multi_SetAIController();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "CharacterBase")
	void Server_RunAttackAnimation();

	UFUNCTION(NetMultiCast, Reliable, BlueprintCallable, Category = "CharacterBase")
	void Multi_RunAttackAnimation();

	UFUNCTION()
	void DeathCharacter(AController* EventInstigator);

	UFUNCTION()
	void ResetCharacter();

	UFUNCTION()
	void HiddenCharacter();

	UFUNCTION()
	void VisibleCharacter();

	UFUNCTION()
	void SellThis();

	UFUNCTION(Server, Reliable)
	void Server_DestroyThis();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UAnimMontage* attackAnimMontage;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UCharacterStatus* status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterBase")
	bool isFollowMouse;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	ATileBase* hasTile;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	APlayerCharacterBase* ownerPawn;

	UPROPERTY()
	FDTCharacterStatus statusData;

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	APlayerCharacterBase* GetOwnerPawn();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "CharacterBase")
	void SetOwnerPawn(APlayerCharacterBase* _owner);

	void SetStatusData(FDTCharacterStatus _statusData, int32 _star = 1);

	UFUNCTION(NetMulticast, Reliable)
	void SetMeshMaterial(int32 _index, UMaterialInterface* _material);

	//delegate
private:
	FCharacterTickDelegate m_tickDelegate;

public:
	UPROPERTY(BlueprintAssignable, Category = "CharacterBase")
	FCharacterMotionEndDelegate motionEndDelegate;

	//Move
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void MoveStraight(ATileBase* _target, float _interval = 5.0f);

protected:
	UFUNCTION()
	void Delegate_MoveStraight();
private:
	FVector m_dir;
	ATileBase* m_preTile;
	float m_interval;

public:
	UFUNCTION(Server, Reliable)
	void Server_SwapTile(ATileBase* _target);

	UFUNCTION(Server, Reliable)
	void Multi_SwapTile(ATileBase* _target);

	//FollowMouse
private:
	UFUNCTION()
	void Delegate_FollowMouse();
	UFUNCTION()
	void ConnectFollowMouse(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	UPROPERTY(Replicated)
	ATileBase* detectTile;

	bool isDestroyed;

	//Widget
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	UWidgetComponent* hpBarWidget;
	
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

