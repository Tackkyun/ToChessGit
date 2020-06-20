// Fill out your copyright notice in the Description page of Project Settings.

#include "ToChess/Public/DeployObject/DeployObjectBase.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
ADeployObjectBase::ADeployObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void ADeployObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeployObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

