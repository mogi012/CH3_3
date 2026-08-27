// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"

#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpawnVolume.h"

#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	CurrentWaveIndex = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	bGameFinished = false;
	bClearItemsOnWaveEnd = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> WaveTable(
		TEXT("/Game/Data/DT_WaveTable.DT_WaveTable"));
	if (WaveTable.Succeeded())
	{
		WaveDataTable = WaveTable.Object;
	}
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	for (AActor* Actor : FoundVolumes)
	{
		if (ASpawnVolume* Volume = Cast<ASpawnVolume>(Actor))
		{
			SpawnVolumes.Add(Volume);
		}
	}

	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("레벨에 SpawnVolume이 없습니다."));
	}

	if (WaveDataTable)
	{
		static const FString ContextString(TEXT("WaveContext"));
		WaveDataTable->GetAllRows<FWaveRow>(ContextString, WaveRows);
	}

	if (WaveRows.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("웨이브 표가 비어 있습니다."));
		return;
	}

	if (USpartaGameInstance* GI = Cast<USpartaGameInstance>(GetGameInstance()))
	{
		GI->ResetGameData();
	}

	StartWave(0);
}

void ASpartaGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void ASpartaGameState::AddScore(int32 Amount)
{
	Score += Amount;

	if (USpartaGameInstance* GI = Cast<USpartaGameInstance>(GetGameInstance()))
	{
		GI->AddToScore(Amount);
	}

	OnScoreChanged.Broadcast(Score);
}

float ASpartaGameState::GetRemainingWaveTime() const
{
	const float Remaining = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
	return Remaining > 0.0f ? Remaining : 0.0f;
}

void ASpartaGameState::ClearAllSpawnVolumes()
{
	for (ASpawnVolume* Volume : SpawnVolumes)
	{
		if (IsValid(Volume))
		{
			Volume->ClearSpawnedActors();
		}
	}
}

void ASpartaGameState::StartWave(int32 WaveIndex)
{
	if (!WaveRows.IsValidIndex(WaveIndex))
	{
		OnGameOver();
		return;
	}

	CurrentWaveIndex = WaveIndex;

	if (bClearItemsOnWaveEnd)
	{
		ClearAllSpawnVolumes();
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	const FWaveRow* Row = WaveRows[WaveIndex];
	UDataTable* ItemTable = Row->ItemSpawnTable.LoadSynchronous();

	for (ASpawnVolume* Volume : SpawnVolumes)
	{
		if (!IsValid(Volume))
		{
			continue;
		}

		for (int32 i = 0; i < Row->ItemToSpawn; ++i)
		{
			AActor* Spawned = Volume->SpawnRandomItem(ItemTable);
			if (Spawned && Spawned->IsA(ACoinItem::StaticClass()))
			{
				SpawnedCoinCount++;
			}
		}

		if (Row->HazardClass)
		{
			for (int32 i = 0; i < Row->HazardCount; ++i)
			{
				Volume->SpawnActorAtRandomPoint(Row->HazardClass);
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		Row->WaveDuration,
		false);

	const int32 WaveNumber = CurrentWaveIndex + 1;

	UE_LOG(LogTemp, Warning, TEXT("Wave %d 시작! 제한 시간 %.0f초, 아이템 %d개, 코인 %d개"),
		WaveNumber, Row->WaveDuration, Row->ItemToSpawn, SpawnedCoinCount);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 3.0f, FColor::Yellow,
			FString::Printf(TEXT("Wave %d Start!  Time %.0fs  Coins %d"),
				WaveNumber, Row->WaveDuration, SpawnedCoinCount));
	}

	OnWaveChanged.Broadcast(WaveNumber, WaveRows.Num());

	const FText Notice = Row->NoticeText.IsEmpty()
		? FText::FromString(FString::Printf(TEXT("Wave %d 시작!"), WaveNumber))
		: Row->NoticeText;
	OnWaveNotice.Broadcast(Notice);
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::EndWave()
{
	if (bGameFinished)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	const int32 NextIndex = CurrentWaveIndex + 1;

	if (WaveRows.IsValidIndex(NextIndex))
	{
		StartWave(NextIndex);
		return;
	}

	bGameFinished = true;
	ClearAllSpawnVolumes();

	UE_LOG(LogTemp, Warning, TEXT("모든 웨이브 완료. 최종 점수 %d"), Score);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("All Waves Cleared!  Score %d"), Score));
	}

	OnGameFinished.Broadcast(true);
}

void ASpartaGameState::OnGameOver()
{
	if (bGameFinished)
	{
		return;
	}
	bGameFinished = true;

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	ClearAllSpawnVolumes();

	UE_LOG(LogTemp, Warning, TEXT("게임 오버. 최종 점수 %d"), Score);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Game Over!  Score %d"), Score));
	}

	OnGameFinished.Broadcast(false);
}