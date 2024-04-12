// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBots_TimeElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBots_TimeElapsed()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if(ensure(QueryInstance))
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	int32 NrOfAliveBots = 0;
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(AttributeComp && AttributeComp->isAlive())
		{
			NrOfAliveBots++;
		}
	}

	float MaxBotCount = 10;
	
	if(DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if(NrOfAliveBots >= MaxBotCount)
		return;
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
	}
}