// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"


class UWorld;
/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent)	
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent)	
	void StopAction(AActor* Instigator);
	
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName;

	UWorld* GetWorld() const override;
};
