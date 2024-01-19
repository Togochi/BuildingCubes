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
	M_CurrentMatIndex = 0;

	if (IsValid(M_Owner) &&
		IsValid(M_Owner->BC_LightSphere) &&
		IsValid(M_Owner->BC_LightSphere->GetMaterial(0)) &&
		IsValid(GetWorld()))
	{
		M_LightSphereMat = UMaterialInstanceDynamic::Create(M_Owner->BC_LightSphere->GetMaterial(0), GetWorld());

		if (M_LightSphereMat)
		{
			M_Owner->BC_LightSphere->SetMaterial(0, M_LightSphereMat);
		}
	}
	
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

		this->CreateAndSetMaterial(this->BlockMaterialPairs[this->M_CurrentMatIndex].Base);
		if (IsValid(M_CurrentMat) && IsValid(M_CurrentBlock))
		{
			M_CurrentMat->SetVectorParameterValue(FName("Emit Color"), this->BlockMaterialPairs[this->M_CurrentMatIndex].EmitColor);
			M_CurrentBlock->OnEndBuilding();
		}
	}
	if (M_CurrentAction == EActionType::Destroy)
	{
		this->M_isStartDestroy = false;
		Cast<ABC_C_BaseBlock>(M_HitActor)->OnResetDestroy();
	}



	UE_LOG(LogBC_BuildingComponent, Display, TEXT("End Action"));
}

void UBC_BuildingComponent::ChangeMaterial(float Value)
{
	this->M_CurrentMatIndex += FMath::Clamp(Value, -1.f, 1.f);
	

	if (this->M_CurrentMatIndex == this->BlockMaterialPairs.Num())
	{
		M_CurrentMatIndex = 0;
	}
	else if (M_CurrentMatIndex == -1)
	{
		M_CurrentMatIndex = this->BlockMaterialPairs.Num() - 1;
	}

	if (this->M_isStartPreview)
	{
		this->CreateAndSetMaterial(this->BlockMaterialPairs[this->M_CurrentMatIndex].Preview);
	}

	UE_LOG(LogBC_BuildingComponent, Display, TEXT("M_CurrentMatIndex %i"), M_CurrentMatIndex);
}

void UBC_BuildingComponent::SwitchAction()
{
	if (M_CurrentAction == EActionType::Destroy || M_CurrentAction == EActionType::None)
	{
		M_CurrentAction = EActionType::Building;
		M_LightSphereMat->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
	}
	else
	{
		M_CurrentAction = EActionType::Destroy;
		M_LightSphereMat->SetVectorParameterValue(FName("Color"), FLinearColor::Red);
	}
}

void UBC_BuildingComponent::ChangeBlock()
{
	M_CurrentBlockIndex++;


	if (M_CurrentBlockIndex == BlocksClasses.Num())
	{
		M_CurrentBlockIndex = 0;
	}
	else if (M_CurrentBlockIndex == -1)
	{
		M_CurrentBlockIndex = this->BlocksClasses.Num() - 1;
	}

	if (this->M_isStartPreview)
	{
		M_CurrentBlock->Destroy();
		M_isStartPreview = false;
		M_isStartBuilding = true;
	}


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
	else if (this->M_isStartDestroy)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this->M_Owner);


		FHitResult HitResult;
		this->DrawTrace(IgnoredActors, HitResult, this->MaxTraceDistance);

		if (HitResult.bBlockingHit && IsValid(HitResult.GetActor()))
		{
			ABC_C_BaseBlock* BaseBlock = Cast<ABC_C_BaseBlock>(HitResult.GetActor());
			if (BaseBlock)
			{
				Cast<ABC_C_BaseBlock>(HitResult.GetActor())->OnStartDestroy();
			}

			if (IsValid(M_HitActor) && M_HitActor->GetName() != HitResult.GetActor()->GetName())
			{
				Cast<ABC_C_BaseBlock>(M_HitActor)->OnResetDestroy();
			}
			M_HitActor = HitResult.GetActor();
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
		EDrawDebugTrace::None,
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
	if (!IsValid(BlocksClasses[M_CurrentBlockIndex])) return false;
	FTransform Transform;
	Transform.SetLocation(HitResult.Location);
	this->M_CurrentBlock = GetWorld()->SpawnActor<ABC_C_BaseBlock>(BlocksClasses[M_CurrentBlockIndex], Transform);
	this->CreateAndSetMaterial(this->BlockMaterialPairs[this->M_CurrentMatIndex].Preview);

	if (IsValid(M_CurrentBlock))
	{
		FVector Origin(ForceInitToZero), Extend(ForceInitToZero);
		M_CurrentBlock->GetActorBounds(false, Origin, Extend);
		M_CurrentBlockExtend = Extend.X;


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
		this->M_BlockLoc = HitResult.Location.GridSnap(M_CurrentBlockExtend) + HitResult.Normal * M_CurrentBlockExtend;
		
	}
	else
	{
		FVector StartLoc(ForceInitToZero), EndLoc(ForceInitToZero);
		this->CalculateStartEndLoc(this->WithoutHitDistance, StartLoc, EndLoc);
		this->M_BlockLoc = EndLoc.GridSnap(M_CurrentBlockExtend);
	}

	const TArray<AActor*> IgnoredActors = { this->M_Owner, this->M_CurrentBlock };
	TArray<FHitResult> BoxHits;

	UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		this->M_BlockLoc, this->M_BlockLoc,
		FVector(M_CurrentBlockExtend),
		FRotator::ZeroRotator,
		TraceTypeQuery1,
		false,
		IgnoredActors,
		EDrawDebugTrace::None,
		BoxHits,
		true);

	for (const auto& OneHit : BoxHits)
	{
		this->M_BlockLoc += OneHit.Normal;
		//UE_LOG(LogBC_BuildingComponent, Display, TEXT("Hit: %s"), *OneHit.Normal.ToString());
	}

	if (!IsValid(M_CurrentBlock)) return;
	M_CurrentBlock->SetActorLocation(this->M_BlockLoc);
}

void UBC_BuildingComponent::CreateAndSetMaterial(UMaterialInterface* ParentMaterial)
{
		if (IsValid(M_CurrentBlock) && IsValid(GetWorld()) && IsValid(ParentMaterial))
		{

			this->M_CurrentMat = UMaterialInstanceDynamic::Create(ParentMaterial, GetWorld());
			M_CurrentBlock->BC_MeshComponent->SetMaterial(0, this->M_CurrentMat);

			M_DeltaIndex = M_CurrentMatIndex;

			UE_LOG(LogBC_BuildingComponent, Display, TEXT("Change Mat"));
		}

}

