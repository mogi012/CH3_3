// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaHUDWidget.h"

#include "SpartaCharacter.h"
#include "SpartaGameState.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void USpartaHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NoticeDuration = 2.5f;
	NoticeText->SetVisibility(ESlateVisibility::Hidden);

	if (ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>())
	{
		GameState->OnScoreChanged.AddDynamic(this, &USpartaHUDWidget::HandleScoreChanged);
		GameState->OnWaveChanged.AddDynamic(this, &USpartaHUDWidget::HandleWaveChanged);
		GameState->OnWaveNotice.AddDynamic(this, &USpartaHUDWidget::HandleWaveNotice);

		HandleScoreChanged(GameState->GetScore());
		HandleWaveChanged(GameState->GetCurrentWaveNumber(), GameState->GetMaxWaves());
	}
}

void USpartaHUDWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NoticeTimerHandle);
	}

	if (ASpartaGameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr)
	{
		GameState->OnScoreChanged.RemoveDynamic(this, &USpartaHUDWidget::HandleScoreChanged);
		GameState->OnWaveChanged.RemoveDynamic(this, &USpartaHUDWidget::HandleWaveChanged);
		GameState->OnWaveNotice.RemoveDynamic(this, &USpartaHUDWidget::HandleWaveNotice);
	}

	Super::NativeDestruct();
}

void USpartaHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ASpartaGameState* GameState = GetWorld()->GetGameState<ASpartaGameState>())
	{
		const float Remaining = FMath::CeilToFloat(GameState->GetRemainingWaveTime());
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Remaining)));
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	ASpartaCharacter* Character = Cast<ASpartaCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}

	const float Health = Character->GetHealth();
	const float MaxHealth = Character->GetMaxHealth();

	HealthBar->SetPercent(MaxHealth > 0.0f ? Health / MaxHealth : 0.0f);
	HealthText->SetText(FText::FromString(
		FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
}

void USpartaHUDWidget::HandleScoreChanged(int32 NewScore)
{
	ScoreText->SetText(FText::FromString(FString::Printf(TEXT("SCORE  %d"), NewScore)));
}

void USpartaHUDWidget::HandleWaveChanged(int32 WaveNumber, int32 MaxWaves)
{
	WaveText->SetText(FText::FromString(
		FString::Printf(TEXT("WAVE  %d / %d"), WaveNumber, MaxWaves)));
}

void USpartaHUDWidget::HandleWaveNotice(const FText& Message)
{
	NoticeText->SetText(Message);
	NoticeText->SetVisibility(ESlateVisibility::HitTestInvisible);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(NoticeTimerHandle);
	World->GetTimerManager().SetTimer(
		NoticeTimerHandle, this, &USpartaHUDWidget::HideNotice, NoticeDuration, false);
}

void USpartaHUDWidget::HideNotice()
{
	NoticeText->SetVisibility(ESlateVisibility::Hidden);
}