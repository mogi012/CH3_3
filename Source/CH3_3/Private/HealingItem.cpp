// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"

#include "SpartaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHealingItem::AHealingItem()
{
	HealAmount = 25.0f;
	ItemType = TEXT("Healing");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PotionMesh(
		TEXT("/Game/Fab/Health_potion/health_potion/StaticMeshes/health_potion.health_potion"));
	if (PotionMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(PotionMesh.Object);
		StaticMesh->SetRelativeScale3D(FVector(0.25f));
	}
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (!Activator || !Activator->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(Activator))
	{
		Character->AddHealth(HealAmount);
	}

	PlayPickupEffects();
	DestroyItem();
}

