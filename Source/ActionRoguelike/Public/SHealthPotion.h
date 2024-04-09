// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SAttributeComponent.h"
#include "SBuffItem.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASHealthPotion : public ASBuffItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASHealthPotion();
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditAnywhere, Category="Stats")
	float healAmount;
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
