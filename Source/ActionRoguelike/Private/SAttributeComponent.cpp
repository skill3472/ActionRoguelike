// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	maxHealth = 100;
	Health = maxHealth;
}


const bool USAttributeComponent::isAlive()
{
	return Health > 0;
	
}

bool USAttributeComponent::ApplyHealthChange(float delta)
{
	float oldHealth = Health;

	Health = FMath::Clamp(Health + delta, 0.0f, maxHealth);
	
	float ActualDelta = Health - oldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);
	
	return ActualDelta != 0;
}

float USAttributeComponent::GetHealthMax()
{
	return maxHealth;
}


