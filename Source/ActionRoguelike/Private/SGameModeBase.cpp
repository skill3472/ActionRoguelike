// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via a timer."), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarBotMaxMultiplier(TEXT("su.BotMultiplier"), 1.0f, TEXT("Change the max bots alive multiplier"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDrawBotSpawnPoint(TEXT("su.DrawBotSpawn"), false, TEXT("Change if you want to see debug spheres at locations that bots spawned at."), ECVF_Cheat);

// ReSharper disable once CppPossiblyUninitializedMember
ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsFromBotKill = 25;
	AmountOfBuffsToSpawn = 5;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	SpawnBuffs();
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
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bots EQS Query Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
		if(CVarDrawBotSpawnPoint.GetValueOnGameThread())
			DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

void ASGameModeBase::SpawnBuffs()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBuffsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
	if(ensure(QueryInstance))
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBuffsQueryCompleted);
}

void ASGameModeBase::OnBuffsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Buffs EQS Query Failed!"));
		return;
	}
	TArray<FVector> AllLocations = QueryInstance->GetResultsAsLocations();
	TArray<FVector> Locations;
	for(int i = 0; i < AmountOfBuffsToSpawn; i++)
	{
		Locations.Add(AllLocations[FMath::RandRange(0, AllLocations.Num()-1)]);
	}
	for (FVector Location : Locations)
	{
		TSubclassOf<AActor> BuffItem = BuffItems[FMath::RandRange(0, BuffItems.Num()-1)];
		GetWorld()->SpawnActor<AActor>(BuffItem, Location, FRotator::ZeroRotator);
	}
}

// Cheat
void ASGameModeBase::KillAll()
{
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		if(USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot))
		{
			if(ensure(AttributeComp) && AttributeComp->isAlive())
			{
				AttributeComp->Kill(this); // @fixme: pass in player for kill credit maybe?
			}
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
	ASAICharacter* Bot = Cast<ASAICharacter>(Victim);
	if(Bot)
	{
		ASCharacter* PlayerKiller = Cast<ASCharacter>(Killer);
		ASPlayerState* PlayerState = Cast<ASPlayerState>(PlayerKiller->GetPlayerState());
		if(ensure(PlayerState))
		{
			PlayerState->ApplyCreditsChange(CreditsFromBotKill);
		}
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