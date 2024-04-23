// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via a timer."), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarBotMaxMultiplier(TEXT("su.BotMultiplier"), 1.0f, TEXT("Change the max bots alive multiplier"), ECVF_Cheat);

// ReSharper disable once CppPossiblyUninitializedMember
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
	if(!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot tried to spawn, but disabled via CVarSpawnBots = false."))
		return;
	}
	
	int32 NrOfAliveBots = 0;
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(ensure(AttributeComp) && AttributeComp->isAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	float MaxBotCount = 10;
	
	if(DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	MaxBotCount *= CVarSpawnBots.GetValueOnGameThread();
	
	if(NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At max bot capacity, skipping bot spawn."));
		return;
	}
	
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
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

// Cheat
void ASGameModeBase::KillAll()
{
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(ensure(AttributeComp) && AttributeComp->isAlive())
		{
			AttributeComp->Kill(this); // @fixme: pass in player for kill credit maybe?
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* Victim, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(Victim);
	if(Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayer_TimeElapsed", Player->GetController());

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s Killer: %s"), *GetNameSafe(Victim), *GetNameSafe(Killer));
}

void ASGameModeBase::RespawnPlayer_TimeElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		Controller->UnPossess();
		
		RestartPlayer(Controller);
	}
}