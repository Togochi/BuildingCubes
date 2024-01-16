// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingCubes/Public/BC_Variables.h"
#include "BC_BuildingComponent.generated.h"

class ABC_C_Character;
class ABC_C_BaseBlock;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGCUBES_API UBC_BuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBC_BuildingComponent();

	void StartAction();
	void EndAction();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Subclass")
	TSubclassOf<AActor> BigBlockClass;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool M_isStartAction;
	EActionType M_CurrentAction;


	UPROPERTY()
	ABC_C_Character* M_Owner;

	UPROPERTY()
	ABC_C_BaseBlock* M_CurrentBlock;

	void DrawTrace(FHitResult& HitResult);
		
};
