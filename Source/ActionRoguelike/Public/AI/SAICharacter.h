// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

protected:

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category="Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* attributeComp;

	UFUNCTION()
	void OnPawnSeen(APawn* pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

};
