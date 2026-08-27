// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WaveRow.h"
#include "SpartaGameState.generated.h"

class ASpawnVolume;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveChanged, int32, WaveNumber, int32, MaxWaves);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveNotice, const FText&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameFinished, bool, bCleared);

UCLASS()
class CH3_3_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveChanged OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveNotice OnWaveNotice;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnGameFinished OnGameFinished;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const { return Score; }

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWaveNumber() const { return CurrentWaveIndex + 1; }

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetMaxWaves() const { return WaveRows.Num(); }

	UFUNCTION(BlueprintPure, Category = "Wave")
	float GetRemainingWaveTime() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCollectedCoinCount() const { return CollectedCoinCount; }

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetSpawnedCoinCount() const { return SpawnedCoinCount; }

	void OnCoinCollected();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void OnGameOver();

protected:
	UPROPERTY()
	UDataTable* WaveDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	bool bClearItemsOnWaveEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 SpawnedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CollectedCoinCount;

	bool bGameFinished;

	TArray<FWaveRow*> WaveRows;

	FTimerHandle WaveTimerHandle;

	UPROPERTY()
	TArray<ASpawnVolume*> SpawnVolumes;

	void StartWave(int32 WaveIndex);
	void OnWaveTimeUp();
	void EndWave();
	void ClearAllSpawnVolumes();
};