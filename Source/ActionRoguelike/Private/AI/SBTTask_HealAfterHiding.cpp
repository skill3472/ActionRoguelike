// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealAfterHiding.h"
#include "AI/SAIController.h"
#include "GameFramework/Character.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type USBTTask_HealAfterHiding::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if(ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if(MyPawn == nullptr)
			return EBTNodeResult::Failed;

		USAttributeComponent* MyAttributes = USAttributeComponent::GetAttributes(MyPawn);
		if(ensure(MyAttributes))
		{
			MyAttributes->SetHealth(MyAttributes->GetHealthMax());
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}


