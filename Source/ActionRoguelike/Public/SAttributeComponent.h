// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Health")
	static bool IsActorAlive(AActor* Actor);
	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Health")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Health")
	float maxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float LowHealthThreshold;

	UPROPERTY(VisibleAnywhere, Category="Rage")
	bool bUsesRage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rage")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rage")
	float MaxRage;

	/* Amount to multiply the damage by, to get the Rage value to add. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rage")
	float DamageToRageMultiplier;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

public:
	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);
	
	UFUNCTION(BlueprintCallable)
	bool isAlive();

	UFUNCTION(BlueprintCallable)
	bool IsLowHealth();
	
	UFUNCTION(BlueprintCallable)
	bool ApplyHealthChange(AActor* InstigatorActor, float delta);

	UFUNCTION(BlueprintCallable)
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRageChanged OnRageChanged;

	UFUNCTION(BlueprintCallable)
	float GetHealthMax();

	/* Tells the Attribute Component if you want to use the rage attribute for this Actor */
	UFUNCTION(BlueprintCallable)
	void UseRage(bool UseRage);

	/* Check the value of bUsesRage */
	UFUNCTION(BlueprintCallable)
	bool UsesRage();

	UFUNCTION(BlueprintCallable)
	float GetRage();
};
