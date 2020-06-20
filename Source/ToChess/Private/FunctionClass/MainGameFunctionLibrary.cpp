// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ToChess/Public/GameMode/GamePlayerController.h"


AGamePlayerController* UMainGameFunctionLibrary::GetMainGamePlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	for (int32 i = 0, index = 0; UGameplayStatics::GetPlayerController(WorldContextObject, i); ++i)
	{
		if (Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, i)))
		{
			if (index == PlayerIndex)
			{
				return Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, i));
			}
			++index;
		}
	}

	return nullptr;
}
