// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMineItem::AMineItem()
{
	ExplosionRadius = 350.0f;
	ExplosionDamage = 30.0f;
	bHasActivated = false;
	ItemType = TEXT("Mine");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BombMesh(
		TEXT("/Game/Assets/CC0_-_Bomb/cc0_bomb/StaticMeshes/cc0_bomb.cc0_bomb"));
	if (BombMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(BombMesh.Object);
		StaticMesh->SetRelativeScale3D(FVector(4.0f));
	}

	Collision->SetSphereRadius(60.0f);
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasActivated)
	{
		return;
	}
	bHasActivated = true;

	PlayPickupEffects();
	Explode();
}

void AMineItem::Explode()
{
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation(), true);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag(TEXT("Player")))
		{
			UGameplayStatics::ApplyDamage(
				Actor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
		}
	}

	DestroyItem();
}