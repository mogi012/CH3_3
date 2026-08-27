// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpartaHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class CH3_3_API USpartaHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoticeText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	float NoticeDuration;

	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	UFUNCTION()
	void HandleWaveChanged(int32 WaveNumber, int32 MaxWaves);

	UFUNCTION()
	void HandleWaveNotice(const FText& Message);

	void HideNotice();

	FTimerHandle NoticeTimerHandle;
};