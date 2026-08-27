// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WaveRow.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FWaveRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "1.0"))
	float WaveDuration = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave", meta = (ClampMin = "0"))
	int32 ItemToSpawn = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSoftObjectPtr<UDataTable> ItemSpawnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Hazard")
	TSubclassOf<AActor> HazardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Hazard", meta = (ClampMin = "0"))
	int32 HazardCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FText NoticeText;
};