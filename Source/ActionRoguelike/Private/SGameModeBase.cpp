// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Engine/AssetManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via a timer."), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarBotMaxMultiplier(TEXT("su.BotMultiplier"), 1.0f, TEXT("Change the max bots alive multiplier"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDrawBotSpawnPoint(TEXT("su.DrawBotSpawn"), false, TEXT("Change if you want to see debug spheres at locations that bots spawned at."), ECVF_Cheat);

// ReSharper disable once CppPossiblyUninitializedMember
ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsFromBotKill = 25;
	AmountOfBuffsToSpawn = 5;

	SlotName = "SaveGame01";
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	LoadSaveGame();
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnBotQueryCompleted);
}

void ASGameModeBase::OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bots EQS Query Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.IsValidIndex(0))
	{
		if(MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			// Random enemy @fixme: add a better system for picking enemies, based on weight
			int32 RandomIndex = FMath::RandRange(0, Rows.Num()-1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfInitialized();
			if(Manager)
			{
				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if(Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if(MonsterData)
		{
			AActor* SpawnedBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if(SpawnedBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(SpawnedBot), *GetNameSafe(MonsterData)));
			
				// Grants buffs etc.
				USActionComponent* BotActionComp = USActionComponent::GetActionComp(SpawnedBot);
				if(BotActionComp)
				{
					for(TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						BotActionComp->AddAction(SpawnedBot, ActionClass);
					}
				}
			}
			
			if(CVarDrawBotSpawnPoint.GetValueOnGameThread())
				DrawDebugSphere(GetWorld(), SpawnLocation, 50.0f, 20, FColor::Blue, false, 60.0f);
		}
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
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(ensure(AttributeComp) && AttributeComp->isAlive())
		{
			AttributeComp->Kill(this); // pass in player for kill credit maybe?
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

void ASGameModeBase::WriteSaveGame()
{
	for(int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // Only single player - we just have one credits var in the save file
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	for(FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if(!Actor->Implements<USGameplayInterface>())
			continue;

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.ActorTransform = Actor->GetActorTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);
		
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		Ar.ArIsSaveGame = true;
		
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}
	
	if(UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0))
		UE_LOG(LogTemp, Log, TEXT("Saved the game successfully !"))
	else
		UE_LOG(LogTemp, Warning, TEXT("There was a problem with saving the game!"))
}

void ASGameModeBase::LoadSaveGame()
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if(CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load save game!"))
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded save game data."))

		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[0]);
		if (PS)
		{
			PS->LoadPlayerState(CurrentSaveGame);
		}
		
		for(FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if(!Actor->Implements<USGameplayInterface>())
				continue;

			for(FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if(ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.ActorTransform);

					FMemoryReader MemReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
		
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					
					break;
				}
			}
		}
		OnSaveLoaded.Broadcast();
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		if(CurrentSaveGame)
			UE_LOG(LogTemp, Log, TEXT("Created new save game data."))
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create new save game!"))
			return;
		}
	}
}
