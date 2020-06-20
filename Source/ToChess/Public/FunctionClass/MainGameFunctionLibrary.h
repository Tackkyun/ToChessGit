// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MainGameFunctionLibrary.generated.h"

class AGamePlayerController;

/**
 * 
 */
UCLASS()
class TOCHESS_API UMainGameFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//�ش� Ŭ������ PlayerController�� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "MainGameFunctionLibrary")
	static AGamePlayerController* GetMainGamePlayerController(const UObject* WorldContextObject, int32 PlayerIndex);
};
