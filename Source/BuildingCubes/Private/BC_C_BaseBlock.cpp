// Fill out your copyright notice in the Description page of Project Settings.


#include "BC_C_BaseBlock.h"


ABC_C_BaseBlock::ABC_C_BaseBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BC_MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BC_MeshComponent");
	SetRootComponent(BC_MeshComponent);
}


void ABC_C_BaseBlock::BeginPlay()
{
	Super::BeginPlay();
}


void ABC_C_BaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

