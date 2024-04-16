// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	maxHealth = 100;
	Health = maxHealth;
	LowHealthThreshold = 50.0f;
}


bool USAttributeComponent::isAlive()
{
	return Health > 0;
	
}

bool USAttributeComponent::IsLowHealth()
{
	return Health <= LowHealthThreshold;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float delta)
{
	float oldHealth = Health;

	Health = FMath::Clamp(Health + delta, 0.0f, maxHealth);
	
	float ActualDelta = Health - oldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);
	
	return ActualDelta != 0;
}

void USAttributeComponent::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, maxHealth);
}

float USAttributeComponent::GetHealthMax()
{
	return maxHealth;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}
	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if(AttributeComp)
	{
		return AttributeComp->isAlive();
	}
	return false;
}
