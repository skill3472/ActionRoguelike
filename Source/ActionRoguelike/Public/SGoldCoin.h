// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBuffItem.h"
#include "SGoldCoin.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGoldCoin : public ASBuffItem
{
	GENERATED_BODY()

public:
	ASGoldCoin();
	
protected:
	bool ApplyBuff(APawn* InstigatorPawn) override;
};
