// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BC_BuildingComponent.h"
#include "BuildingCubes/Public/BC_C_BaseBlock.h"
#include "Camera/CameraComponent.h"
#include "Character/BC_C_Character.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogBC_BuildingComponent, All, All);

UBC_BuildingComponent::UBC_BuildingComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UBC_BuildingComponent::BeginPlay()
{
	Super::BeginPlay();

	M_Owner = Cast<ABC_C_Character>(GetOwner());
	M_CurrentAction = EActionType::Building;
	
}

void UBC_BuildingComponent::StartAction()
{

	if (M_CurrentAction == EActionType::Building)
	{
		this->M_isStartBuilding = true;
	}
	if (M_CurrentAction == EActionType::Destroy)
	{
		this->M_isStartDestroy = true;
	}

	UE_LOG(LogBC_BuildingComponent, Display, TEXT("Start Action"));
}

void UBC_BuildingComponent::EndAction()
{

	if (M_CurrentAction == EActionType::Building)
	{
		this->M_isStartBuilding = false;
		this->M_isStartPreview = false;

		if (IsValid(M_CurrentBlock) && IsValid(GetWorld()) && IsValid(this->BlockMaterialPairs[0].Base))
		{
			if (!this->M_CurrentBaseMat)
			{
				this->M_CurrentBaseMat = UMaterialInstanceDynamic::Create(this->BlockMaterialPairs[0].Base, GetWorld());
			}
			M_CurrentBlock->BC_MeshComponent->SetMaterial(0, this->M_CurrentBaseMat);
		}
	}
	if (M_CurrentAction == EActionType::Destroy)
	{
		this->M_isStartDestroy = true;
	}



	UE_LOG(LogBC_BuildingComponent, Display, TEXT("End Action"));
}


void UBC_BuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->M_isStartBuilding)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this->M_Owner);
		IgnoredActors.AddUnique(this->M_CurrentBlock);

		FHitResult HitResult;
		this->DrawTrace(IgnoredActors, HitResult, this->MaxTraceDistance);

		if (this->CreateBlock(HitResult))
		{
			this->SetBlockLocation(HitResult);
		}	
	
	}
}

void UBC_BuildingComponent::DrawTrace(TArray<AActor*> IgnoredActors, FHitResult& HitResult, float MaxDiatance)
{

	if (!IsValid(this->M_Owner)) return;

	FVector StartLoc(ForceInitToZero), EndLoc(ForceInitToZero);
	this->CalculateStartEndLoc(MaxDiatance, StartLoc, EndLoc);


	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		TraceTypeQuery1,
		false,
		IgnoredActors,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.5f
	);
}

bool UBC_BuildingComponent::CreateBlock(const FHitResult& HitResult)
{
	if (this->M_isStartPreview) return true;
	if (!IsValid(BigBlockClass)) return false;
	FTransform Transform;
	Transform.SetLocation(HitResult.Location);
	this->M_CurrentBlock = GetWorld()->SpawnActor<ABC_C_BaseBlock>(BigBlockClass, Transform);

	if (IsValid(M_CurrentBlock) && IsValid(GetWorld()) && IsValid(this->BlockMaterialPairs[0].Preview))
	{
		if (!this->M_CurrentPreviewMat)
		{
			this->M_CurrentPreviewMat = UMaterialInstanceDynamic::Create(this->BlockMaterialPairs[0].Preview, GetWorld());
		}
		M_CurrentBlock->BC_MeshComponent->SetMaterial(0, this->M_CurrentPreviewMat);
	}
	return this->M_isStartPreview = IsValid(this->M_CurrentBlock);
}

void UBC_BuildingComponent::CalculateStartEndLoc(float Distance, FVector& StartLoc, FVector& EndLoc)
{
	 StartLoc = M_Owner->BC_LightSphere->GetComponentLocation();
	 EndLoc = StartLoc + M_Owner->FindComponentByClass<UCameraComponent>()->GetForwardVector() * Distance;

}

void UBC_BuildingComponent::SetBlockLocation(const FHitResult& HitResult)
{

	if (HitResult.bBlockingHit)
	{
		this->M_BlockLoc = HitResult.Location.GridSnap(50.f) + HitResult.Normal * 50.f;
		
	}
	else
	{
		FVector StartLoc(ForceInitToZero), EndLoc(ForceInitToZero);
		this->CalculateStartEndLoc(this->WithoutHitDistance, StartLoc, EndLoc);
		this->M_BlockLoc = EndLoc.GridSnap(50.f);
	}

	const TArray<AActor*> IgnoredActors = { this->M_Owner, this->M_CurrentBlock };
	TArray<FHitResult> BoxHits;

	UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		this->M_BlockLoc, this->M_BlockLoc,
		FVector(50.f),
		FRotator::ZeroRotator,
		TraceTypeQuery1,
		false,
		IgnoredActors,
		EDrawDebugTrace::ForOneFrame,
		BoxHits,
		true);

	for (const auto& OneHit : BoxHits)
	{
		this->M_BlockLoc += OneHit.Normal;
		UE_LOG(LogBC_BuildingComponent, Display, TEXT("Hit: %s"), *OneHit.Normal.ToString());
	}
	M_CurrentBlock->SetActorLocation(this->M_BlockLoc);
}

