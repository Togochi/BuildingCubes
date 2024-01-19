#include "BC_C_BaseBlock.h"
#include "BC_C_BaseBlock.h"
#include "BC_C_BaseBlock.h"




ABC_C_BaseBlock::ABC_C_BaseBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BC_MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BC_MeshComponent");
	SetRootComponent(BC_MeshComponent);
}

void ABC_C_BaseBlock::OnEndBuilding()
{
	M_StartFadeEmit = true;

	if (IsValid(BC_MeshComponent) && IsValid(BC_MeshComponent->GetMaterial(0)))
	{
		M_BlockDynMat = Cast<UMaterialInstanceDynamic>(BC_MeshComponent->GetMaterial(0));

	}

}

void ABC_C_BaseBlock::OnStartDestroy()
{
	M_ResetRiseEmit = false;
	M_StartFadeEmit = false;
	M_StartRiseEmit = true;
}

void ABC_C_BaseBlock::OnResetDestroy()
{
	if (!IsValid(this)) return;
	M_StartFadeEmit = true;
	M_StartRiseEmit = false;
	M_ResetRiseEmit = true;
}


void ABC_C_BaseBlock::BeginPlay()
{
	Super::BeginPlay();
	M_FXEmitPower = FadeTime;
}


void ABC_C_BaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (M_StartFadeEmit)
	{
		if (M_FXEmitPower > 0)
		{
			M_FXEmitPower -= DeltaTime;
			const float power = M_FXEmitPower * (M_ResetRiseEmit ? RisePowerMult : FadePowerMult);
			ChangeEmitPower(power);
		}
		else
		{
			ResetEmitToZero();
		}

	}
	else if (M_StartRiseEmit)
	{
		if (M_FXEmitPower < RiseTime)
		{
			M_FXEmitPower += DeltaTime;
			ChangeEmitPower(M_FXEmitPower * RisePowerMult);
		}
		else
		{
			Destroy();
		}


	}
}

void ABC_C_BaseBlock::ChangeEmitPower(float Power)
{
	if (!IsValid(M_BlockDynMat)) return;
	M_BlockDynMat->SetScalarParameterValue(FName("Emit Power"), Power * Power);
}

void ABC_C_BaseBlock::ResetEmitToZero()
{
	ChangeEmitPower(0.f);
	M_FXEmitPower = 0.f;
	M_StartFadeEmit = false;
}

