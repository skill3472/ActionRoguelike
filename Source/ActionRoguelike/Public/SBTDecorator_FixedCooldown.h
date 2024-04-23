// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Cooldown.h"
#include "SBTDecorator_FixedCooldown.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTDecorator_FixedCooldown : public UBTDecorator_Cooldown
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bStartCooledDown;

	void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
};
