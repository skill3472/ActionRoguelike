// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SBuffItem.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASBuffItem : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBuffItem();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditAnywhere, Category="Stats")
	float cooldown;

	bool bIsEnabled;
	
	FTimerHandle TimerHandle_BuffCooldown;

	UFUNCTION()
	virtual void Interact_Implementation(APawn* InstigatorPawn);

	UFUNCTION()
	virtual void Cooldown_TimeElapsed();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
