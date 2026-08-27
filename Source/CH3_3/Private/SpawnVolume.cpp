// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	bSnapToGround = true;
	GroundOffset = 60.0f;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetBoxExtent(FVector(1500.0f, 1500.0f, 200.0f));
	SpawningBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawningBox->SetupAttachment(Scene);
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	const FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	const FVector BoxOrigin = SpawningBox->GetComponentLocation();

	FVector Point = BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));

	if (!bSnapToGround)
	{
		return Point;
	}

	FHitResult Hit;
	const FVector TraceStart = Point + FVector(0.0f, 0.0f, BoxExtent.Z + 500.0f);
	const FVector TraceEnd = Point - FVector(0.0f, 0.0f, 5000.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		Point = Hit.ImpactPoint + FVector(0.0f, 0.0f, GroundOffset);
	}

	return Point;
}

FItemSpawnRow* ASpawnVolume::PickRandomRow(UDataTable* ItemTable) const
{
	if (!ItemTable)
	{
		return nullptr;
	}

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemTable->GetAllRows<FItemSpawnRow>(ContextString, AllRows);

	if (AllRows.Num() == 0)
	{
		return nullptr;
	}

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	if (TotalChance <= 0.0f)
	{
		return nullptr;
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float Accumulated = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		Accumulated += Row->SpawnChance;
		if (RandValue <= Accumulated)
		{
			return Row;
		}
	}

	return AllRows.Last();
}

AActor* ASpawnVolume::SpawnRandomItem(UDataTable* ItemTable)
{
	if (FItemSpawnRow* SelectedRow = PickRandomRow(ItemTable))
	{
		return SpawnActorAtRandomPoint(SelectedRow->ItemClass);
	}

	return nullptr;
}

AActor* ASpawnVolume::SpawnActorAtRandomPoint(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(
		ActorClass, GetRandomPointInVolume(), FRotator::ZeroRotator, SpawnParams);

	if (Spawned)
	{
		SpawnedActors.Add(Spawned);
	}

	return Spawned;
}

void ASpawnVolume::ClearSpawnedActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	SpawnedActors.Empty();
}