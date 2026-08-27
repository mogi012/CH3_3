// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, JumpAction(nullptr)
	, LookAction(nullptr)
	, SprintAction(nullptr)
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(
		TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (IMC.Succeeded())
	{
		InputMappingContext = IMC.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(
		TEXT("/Game/Input/Actions/IA_Move.IA_Move"));
	if (IA_Move.Succeeded())
	{
		MoveAction = IA_Move.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(
		TEXT("/Game/Input/Actions/IA_Jump.IA_Jump"));
	if (IA_Jump.Succeeded())
	{
		JumpAction = IA_Jump.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(
		TEXT("/Game/Input/Actions/IA_Look.IA_Look"));
	if (IA_Look.Succeeded())
	{
		LookAction = IA_Look.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Sprint(
		TEXT("/Game/Input/Actions/IA_Sprint.IA_Sprint"));
	if (IA_Sprint.Succeeded())
	{
		SprintAction = IA_Sprint.Object;
	}
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}