// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaPlayerController.h"

#include "MenuGameMode.h"
#include "SpartaGameOverWidget.h"
#include "SpartaGameState.h"
#include "SpartaHUDWidget.h"
#include "SpartaMainMenuWidget.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, JumpAction(nullptr)
	, LookAction(nullptr)
	, SprintAction(nullptr)
	, HUDWidget(nullptr)
	, MainMenuWidget(nullptr)
	, GameOverWidget(nullptr)
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

	static ConstructorHelpers::FClassFinder<USpartaHUDWidget> HUDBP(
		TEXT("/Game/UI/WBP_HUD"));
	if (HUDBP.Succeeded())
	{
		HUDWidgetClass = HUDBP.Class;
	}

	static ConstructorHelpers::FClassFinder<USpartaMainMenuWidget> MainMenuBP(
		TEXT("/Game/UI/WBP_MainMenu"));
	if (MainMenuBP.Succeeded())
	{
		MainMenuWidgetClass = MainMenuBP.Class;
	}

	static ConstructorHelpers::FClassFinder<USpartaGameOverWidget> GameOverBP(
		TEXT("/Game/UI/WBP_GameOver"));
	if (GameOverBP.Succeeded())
	{
		GameOverWidgetClass = GameOverBP.Class;
	}
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetAuthGameMode<AMenuGameMode>())
	{
		ShowMainMenu();
		return;
	}

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

	ShowHUD();

	if (ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>())
	{
		GameState->OnGameFinished.AddDynamic(
			this, &ASpartaPlayerController::HandleGameFinished);
	}
}

void ASpartaPlayerController::ShowMainMenu()
{
	if (!MainMenuWidgetClass)
	{
		return;
	}

	MainMenuWidget = CreateWidget<USpartaMainMenuWidget>(this, MainMenuWidgetClass);
	if (!MainMenuWidget)
	{
		return;
	}

	MainMenuWidget->AddToViewport();

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ASpartaPlayerController::ShowHUD()
{
	if (!HUDWidgetClass)
	{
		return;
	}

	HUDWidget = CreateWidget<USpartaHUDWidget>(this, HUDWidgetClass);
	if (!HUDWidget)
	{
		return;
	}

	HUDWidget->AddToViewport();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ASpartaPlayerController::HandleGameFinished(bool bCleared)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	if (!GameOverWidgetClass)
	{
		return;
	}

	GameOverWidget = CreateWidget<USpartaGameOverWidget>(this, GameOverWidgetClass);
	if (!GameOverWidget)
	{
		return;
	}

	GameOverWidget->AddToViewport();

	int32 FinalScore = 0;
	if (ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>())
	{
		FinalScore = GameState->GetScore();
	}
	GameOverWidget->SetResult(bCleared, FinalScore);

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}