// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, APawn*, MyPawn);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/* Gets called when the Player Pawn changes (on respawn for example) */
	UPROPERTY(BlueprintAssignable)
	FOnPlayerChanged OnPlayerChanged;

protected:
	
	virtual void SetPawn(APawn* InPawn) override;
};
