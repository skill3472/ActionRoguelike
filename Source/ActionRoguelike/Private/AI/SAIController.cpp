// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensureMsgf(bt, TEXT("BehaviorTree is a nullptr! Please assign the Behavior Tree (bt) to your AI Controller.")))
		RunBehaviorTree(bt);
}
