// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class TOCHESS_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGameHUD(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
private:
	TMap<FString, UUserWidget*> uiAssortment;

public:
	UPROPERTY(BlueprintReadWrite, Category = "GameHUD")
	UUserWidget* mainUI;

public:
	void SetMainUI();

private:
	bool isExecuted;

public:
	bool IsExecuted();

public:
	void VisibleMainUI();
	void HiddenMainUI();

	bool AddUI(UUserWidget* _widget, FString _uiName);
	bool VisibleUI(FString _uiName);
	bool HiddenUI(FString _uiName);
	bool DeleteUI(FString _uiName);
	UUserWidget* FindUI(FString _uiName);
};
