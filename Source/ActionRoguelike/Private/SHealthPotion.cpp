// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	healAmount = 30;
}

bool ASHealthPotion::ApplyBuff(APawn* InstigatorPawn)
{
	USAttributeComponent* attributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(attributeComp)
	{
		if(attributeComp->ApplyHealthChange(this, healAmount))
		{
			return true;
		}
	}
	return false;
}

FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if(AttributeComp && AttributeComp->IsFullHealth())
	{
		return NSLOCTEXT("InteractableActors", "HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(NSLOCTEXT("InteractableActors", "HealthPotion_InteractMessage", "Costs {0} Credits. Restores full health."), CreditsPrice);
}