// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class CH3_3_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;

	virtual void ActivateItem(AActor* Activator) override;
};

UCLASS()
class CH3_3_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
	ABigCoinItem();
};

UCLASS()
class CH3_3_API ASmallCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
	ASmallCoinItem();
};
