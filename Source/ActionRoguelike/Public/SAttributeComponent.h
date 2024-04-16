// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static bool IsActorAlive(AActor* Actor);
	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float maxHealth;

public:
	UFUNCTION(BlueprintCallable)
	const bool isAlive();
	
	UFUNCTION(BlueprintCallable)
	bool ApplyHealthChange(AActor* InstigatorActor, float delta);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable)
	float GetHealthMax();
};
