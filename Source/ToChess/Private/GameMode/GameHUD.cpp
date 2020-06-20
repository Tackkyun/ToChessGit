// Fill out your copyright notice in the Description page of Project Settings.


#include "ToChess/Public/GameMode/GameHUD.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"

AGameHUD::AGameHUD(const FObjectInitializer& ObjectInitializer) : isExecuted(false)
{

}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	//SetMainUI();
	
	//Cast<AGamePlayerController>(GetOwner())->afterAllUserReadyDelegate.AddDynamic(this, &AGameHUD::SetMainUI);
}

void AGameHUD::SetMainUI()
{
	isExecuted = true;

	if (mainUI != nullptr)
	{
		mainUI->RemoveFromViewport();
		mainUI = nullptr;
	}

	UClass* tempWidgetClass = LoadObject<UClass>(this, TEXT("/Game/UI/MainWidget.MainWidget_C"));
	if (tempWidgetClass)
	{
		mainUI = CreateWidget<UUserWidget>(GetWorld(), tempWidgetClass);
		if (mainUI)
		{
			mainUI->AddToViewport();
			VisibleMainUI();
		}
	}
}

bool AGameHUD::IsExecuted()
{
	return isExecuted;
}

void AGameHUD::VisibleMainUI()
{
	mainUI->SetVisibility(ESlateVisibility::Visible);
}

void AGameHUD::HiddenMainUI()
{
	mainUI->SetVisibility(ESlateVisibility::Hidden);
}


bool AGameHUD::AddUI(UUserWidget* _widget, FString _uiName)
{
	if (_widget == nullptr || _uiName.Len() < 1) return false;
	_widget->SetVisibility(ESlateVisibility::Hidden);
	uiAssortment.Add(_uiName, _widget);

	return true;
}

bool AGameHUD::VisibleUI(FString _uiName)
{
	if (uiAssortment.Find(_uiName) == nullptr) return false;

	(*uiAssortment.Find(_uiName))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	return true;
}

bool AGameHUD::HiddenUI(FString _uiName)
{
	if (uiAssortment.Find(_uiName) == nullptr) return false;

	(*uiAssortment.Find(_uiName))->SetVisibility(ESlateVisibility::Hidden);
	return true;
}

bool AGameHUD::DeleteUI(FString _uiName)
{
	if (uiAssortment.Find(_uiName) == nullptr) return false;

	uiAssortment.Remove(_uiName);

	return true;
}

UUserWidget* AGameHUD::FindUI(FString _uiName)
{
	return *uiAssortment.Find(_uiName);
}


