// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"

#include "SpartaPlayerController.h"
#include "GameFramework/DefaultPawn.h"

AMenuGameMode::AMenuGameMode()
{
	DefaultPawnClass = ADefaultPawn::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
}