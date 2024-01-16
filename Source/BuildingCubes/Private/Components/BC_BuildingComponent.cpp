// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BC_BuildingComponent.h"
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
	
}

void UBC_BuildingComponent::StartAction()
{
	FHitResult HitResult;

	UE_LOG(LogBC_BuildingComponent, Display, TEXT("Start Action"));
	if (HitResult.bBlockingHit)
	{
		UE_LOG(LogBC_BuildingComponent, Display, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
	}
}

void UBC_BuildingComponent::EndAction()
{
	UE_LOG(LogBC_BuildingComponent, Display, TEXT("End Action"));

}

void UBC_BuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBC_BuildingComponent::DrawTrace(FHitResult& HitResult)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this->M_Owner);

	if (!IsValid(this->M_Owner)) return;
	const FVector StartLoc = M_Owner->BC_LightSphere->GetComponentLocation();
	FVector EndLoc = StartLoc + M_Owner->FindComponentByClass<UCameraComponent>()->GetForwardVector() * 5000.f;

	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLoc,
		EndLoc,
		TraceTypeQuery1,
		false,
		IgnoredActors,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.5f
	);
}

