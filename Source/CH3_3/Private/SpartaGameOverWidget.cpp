// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameOverWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void USpartaGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RestartButton->OnClicked.AddDynamic(this, &USpartaGameOverWidget::OnRestartClicked);
	MainMenuButton->OnClicked.AddDynamic(this, &USpartaGameOverWidget::OnMainMenuClicked);
}

void USpartaGameOverWidget::SetResult(bool bCleared, int32 FinalScore)
{
	ResultText->SetText(bCleared
		? FText::FromString(TEXT("ALL WAVES CLEARED"))
		: FText::FromString(TEXT("GAME OVER")));

	ResultText->SetColorAndOpacity(bCleared
		? FSlateColor(FLinearColor(1.0f, 0.78f, 0.25f))
		: FSlateColor(FLinearColor(0.85f, 0.25f, 0.2f)));

	FinalScoreText->SetText(FText::FromString(
		FString::Printf(TEXT("FINAL SCORE  %d"), FinalScore)));
}

void USpartaGameOverWidget::OnRestartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("DefaultMap")));
}

void USpartaGameOverWidget::OnMainMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("MenuMap")));
}