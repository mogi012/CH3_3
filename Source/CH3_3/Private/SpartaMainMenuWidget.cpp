// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaMainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void USpartaMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &USpartaMainMenuWidget::OnStartClicked);
	QuitButton->OnClicked.AddDynamic(this, &USpartaMainMenuWidget::OnQuitClicked);
}

void USpartaMainMenuWidget::OnStartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("DefaultMap")));
}

void USpartaMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}