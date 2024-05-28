// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SCharacter.h"
#include "SPlayerState.h"
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

	UPROPERTY(EditAnywhere, Category="Stats")
	int CreditsPrice;

	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive;

	UFUNCTION()
	virtual void Cooldown_TimeElapsed();

	UFUNCTION()
	virtual bool ApplyBuff(APawn* InstigatorPawn);

	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	void SetPowerupState(bool bNewIsActive);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	virtual void Interact_Implementation(APawn* InstigatorPawn);

	FText GetInteractText_Implementation(APawn* InstigatorPawn);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
