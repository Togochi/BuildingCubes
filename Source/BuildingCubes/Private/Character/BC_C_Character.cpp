// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BC_C_Character.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBC_Character, All, All);

ABC_C_Character::ABC_C_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	BC_CameraComponent = CreateDefaultSubobject<UCameraComponent>("BC_CameraComponent");
	BC_CameraComponent->SetupAttachment(GetRootComponent());

	BC_FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BC_FirstPersonMesh");
	BC_FirstPersonMesh->SetupAttachment(BC_CameraComponent);
}


void ABC_C_Character::BeginPlay()
{
	Super::BeginPlay();
	check(BC_CameraComponent);
	check(BC_FirstPersonMesh);
}

void ABC_C_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABC_C_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("Move Forward / Backward"), this, &ABC_C_Character::MoveForward);
	PlayerInputComponent->BindAxis(FName("Move Right / Left"), this, &ABC_C_Character::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn Right / Left Mouse"), this, &ABC_C_Character::AddControllerYawInput);
	PlayerInputComponent->BindAxis(FName("Look Up / Down Mouse"), this, &ABC_C_Character::AddControllerPitchInput);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ABC_C_Character::Jump);
	PlayerInputComponent->BindAction(FName("PrimaryAction"), IE_Pressed, this, &ABC_C_Character::StartAction);
	PlayerInputComponent->BindAction(FName("PrimaryAction"), IE_Released, this, &ABC_C_Character::EndAction);
}

void ABC_C_Character::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ABC_C_Character::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ABC_C_Character::StartAction()
{
	UE_LOG(LogBC_Character, Display, TEXT("Start Action"));
}

void ABC_C_Character::EndAction()
{
	UE_LOG(LogBC_Character, Display, TEXT("End Action"));
}

