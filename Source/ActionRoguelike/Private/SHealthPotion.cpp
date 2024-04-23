// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	healAmount = 30;
}

bool ASHealthPotion::ApplyBuff(APawn* InstigatorPawn)
{
	USAttributeComponent* attributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if(attributeComp)
	{
		if(attributeComp->ApplyHealthChange(this, healAmount))
		{
			return true;
		}
	}
	return false;
}

