// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SBTTask_RangedAttack.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> ProjectileClass;
};
