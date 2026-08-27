// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;
class UDataTable;

UCLASS()
class CH3_3_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem(UDataTable* ItemTable);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnActorAtRandomPoint(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void ClearSpawnedActors();

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bSnapToGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float GroundOffset;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	FItemSpawnRow* PickRandomRow(UDataTable* ItemTable) const;
};
