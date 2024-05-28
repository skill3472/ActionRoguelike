// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBuffItem.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "AI/SAICharacter.h"
#include "SGameModeBase.generated.h"


class USMonsterData;
class UEnvQuery;
class UCurveFloat;
class USSaveGame;
class UDataTable;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveLoaded);

// DataTable Row for spawning monsters in GameMode
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY();

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;

	// Relative chance to pick this monster
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	// Points required to spend by gamemode to spawn this unit.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	// Amount of credits gained for killing this monster
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 KillReward;
};

UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	UPROPERTY(BlueprintAssignable)
	FOnSaveLoaded OnSaveLoaded;

	// virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
protected:
	FTimerHandle TimerHandle_SpawnBots;

	// UPROPERTY(EditDefaultsOnly, Category="AI")
	// TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	float RespawnDelay;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	int CreditsFromBotKill;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	UEnvQuery* SpawnBuffsQuery;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TArray<TSubclassOf<AActor>> BuffItems;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	int AmountOfBuffsToSpawn;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	UPROPERTY()
	FString SlotName;

	// All available monsters.
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UDataTable* MonsterTable;

	UFUNCTION()
	void RespawnPlayer_TimeElapsed(AController* Controller);

	UFUNCTION()
	void SpawnBots_TimeElapsed();

	UFUNCTION()
	void OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	UFUNCTION()
	void SpawnBuffs();

	UFUNCTION()
	void OnBuffsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};
