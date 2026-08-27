// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"

#include "SpartaGameState.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ACoinItem::ACoinItem()
{
	PointValue = 10;
	ItemType = TEXT("Coin");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CoinMesh(
		TEXT("/Game/Assets/Coin/coin/StaticMeshes/coin.coin"));
	if (CoinMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(CoinMesh.Object);
	}
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	if (!Activator || !Activator->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	PlayPickupEffects();

	if (UWorld* World = GetWorld())
	{
		if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
		{
			GameState->AddScore(PointValue);
			GameState->OnCoinCollected();
		}
	}

	DestroyItem();
}

ABigCoinItem::ABigCoinItem()
{
	PointValue = 50;
	ItemType = TEXT("BigCoin");

	StaticMesh->SetRelativeScale3D(FVector(1.5f));
	Collision->SetSphereRadius(110.0f);
}

ASmallCoinItem::ASmallCoinItem()
{
	PointValue = 10;
	ItemType = TEXT("SmallCoin");

	StaticMesh->SetRelativeScale3D(FVector(0.8f));
	Collision->SetSphereRadius(80.0f);

}

