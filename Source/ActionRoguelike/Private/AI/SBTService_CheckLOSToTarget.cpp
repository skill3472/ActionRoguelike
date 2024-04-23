// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckLOSToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckLOSToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* bbc = OwnerComp.GetBlackboardComponent();
	if(ensure(bbc))
	{
		AActor* TargetActor = Cast<AActor>(bbc->GetValueAsObject("TargetActor"));
		if(TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			if(ensure(MyController))
			{
				APawn* AIPawn = MyController->GetPawn();
				if(ensure(AIPawn))
				{
					bool bHasLineOfSight = MyController->LineOfSightTo(TargetActor);
					bbc->SetValueAsBool(HasLOSKey.SelectedKeyName, bHasLineOfSight);
				}
			}
		}
	}
}
