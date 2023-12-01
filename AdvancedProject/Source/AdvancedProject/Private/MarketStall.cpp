// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketStall.h"

#include "Components/BillboardComponent.h"

AMarketStall::AMarketStall()
{
	PrimaryActorTick.bCanEverTick = true;

	actorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Actor Mesh");
	RootComponent = actorMesh;
	editorVisual = CreateDefaultSubobject<UBillboardComponent>("Editor Visual");
	editorVisual->SetupAttachment(GetRootComponent());
}

void AMarketStall::BeginPlay()
{
	Super::BeginPlay();

}

void AMarketStall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMarketStall::InitMarketStall(UStaticMesh* _newMesh, FMarketStallSaveData _saveData)
{
	stallTypes = _saveData.GetSiteType();
	actorMesh->SetStaticMesh(_newMesh);
}


FMarketStallSaveData AMarketStall::GetStallSaveData()
{
	return FMarketStallSaveData(stallTypes);
}
