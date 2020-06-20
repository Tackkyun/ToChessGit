// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "CharacterAIconBase.generated.h"

class ACharacterBase;

/**
 * 
 */
UCLASS()
class TOCHESS_API ACharacterAIconBase : public AAIController
{
	GENERATED_BODY()
	
public:
	ACharacterAIconBase();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	bool IsEnemyInSelfRange();
	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	bool IsEnemyInRange(float _range);
	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	bool FindTargetAndMove();
	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	bool IsEnemyInInfiniteRange();

	virtual void ToMove(FVector _position);
	virtual void StopMove();

	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "CharacterAIconBase")
	ACharacter* GetOwnerChracter();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAIconBase")
	ACharacterBase* targetEnemy;
};
