// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100;
}


bool USAttributeComponent::ApplyHealthChange(float delta)
{
	Health += delta;
	OnHealthChanged.Broadcast(nullptr, this, Health, delta);
	return true;
}


