// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpartaGameOverWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class CH3_3_API USpartaGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetResult(bool bCleared, int32 FinalScore);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FinalScoreText;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();
};