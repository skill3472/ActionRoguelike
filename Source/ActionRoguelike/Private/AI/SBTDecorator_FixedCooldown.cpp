// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTDecorator_FixedCooldown.h"

void USBTDecorator_FixedCooldown::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	if (InitType == EBTMemoryInit::Initialize)
	{
		DecoratorMemory->LastUseTimestamp = bStartCooledDown ?  DBL_MIN : -CoolDownTime;
	}

	DecoratorMemory->bRequestedRestart = false;
}
