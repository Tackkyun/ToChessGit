// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "PlayerCharacterBase.generated.h"

class ATileCluster;
class UCharacterManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPostLoginDelegate);


UCLASS()
class TOCHESS_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//Set Network for UObjects
	//virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION()
	void AfterConnect();

protected:
	bool isPlayer;

public:

	UPROPERTY(BlueprintAssignable, Category = "PlayerCharacterBase")
	FPostLoginDelegate postLoginDelegate;

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "PlayerCharacterBase")
	void Client_BroadCastPostLogin();
	
public:
	UFUNCTION()
	void Attack();

	//**Cluster
public:
	UPROPERTY(Replicated)
	ATileCluster* tileCluster;

private:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SetHomeCluster)
	ATileCluster* homeCluster;
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	UCharacterManager* characterManager;

private:
	UFUNCTION()
	void OnRep_SetHomeCluster();

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerController")
	void SetCluster(ATileCluster* _cluster);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "GamePlayerController")
	void Multi_SetCurrentCluster(ATileCluster* _cluster);

	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	ATileCluster* GetTileCluster();

	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	ATileCluster* GetHomeTileCluster();

	UFUNCTION(BlueprintCallable, Category = "GamePlayerController")
	UCharacterManager* GetCharacterManager();

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GamePlayerController")
	void SpawnCharacter(int32 _num);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerCharacterBase")
	USpringArmComponent* cameraSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerCharacterBase")
	UCameraComponent* camera;

public:
	//Use Server only.
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void SetMoveForMatch(ATileCluster* _targetCluster);

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void MoveForMatch();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void MoveForExit();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "PlayerCharacterBase")
	void Multi_SetPlayerPosition(FTransform _t);

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void BindDelegateProperty();

private:
	UPROPERTY()
	bool bCanSpawnCharacter;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	bool CanSpawnCharacter();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void OnCanSpawnCharacter();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void OffCanSpawnCharacter();

private:
	UPROPERTY()
	bool bCanUpgradeFieldCharacter;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	bool CanUpgradeFieldCharacter();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void OnCanUpgradeFieldCharacter();

	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void OffCanUpgradeFieldCharacter();

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacterBase")
	void CheckUnitUpgrade();

protected:
	bool useBind;

};
