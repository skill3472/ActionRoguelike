// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, APlayerState*, PlayerState, int, Delta, int, NewCredits);

UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category="Credits")
	int32 PlayerCredits;


public:
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool ApplyCreditsChange(int Delta);

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool HasCredits(int Amount);

	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits();
	
	UPROPERTY(BlueprintAssignable)
	FOnCreditsChanged OnCreditsChanged;
};
