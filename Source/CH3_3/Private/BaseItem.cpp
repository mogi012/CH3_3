// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemType = TEXT("None");
	bUseColorOverride = false;
	ItemColor = FLinearColor::White;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(90.0f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ItemBaseMat(
		TEXT("/Game/Materials/M_ItemBase.M_ItemBase"));
	if (ItemBaseMat.Succeeded())
	{
		ColorOverrideMaterial = ItemBaseMat.Object;
	}

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	if (!bUseColorOverride || !ColorOverrideMaterial || !StaticMesh->GetStaticMesh())
	{
		return;
	}

	const int32 SlotCount = StaticMesh->GetNumMaterials();
	for (int32 i = 0; i < SlotCount; ++i)
	{
		StaticMesh->SetMaterial(i, ColorOverrideMaterial);

		if (UMaterialInstanceDynamic* Dynamic = StaticMesh->CreateDynamicMaterialInstance(i))
		{
			Dynamic->SetVectorParameterValue(TEXT("BaseColor"), ItemColor);
		}
	}
}

void ABaseItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
	{
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::PlayPickupEffects()
{
	if (PickupParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), PickupParticle, GetActorLocation(), GetActorRotation(), true);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}
}

void ABaseItem::DestroyItem()
{
	Destroy();
}

