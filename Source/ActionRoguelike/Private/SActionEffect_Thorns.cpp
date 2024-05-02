// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"


USActionEffect_Thorns::USActionEffect_Thorns()
{
	DamageMultiplier = 0.2f;
	ActionName = "Thorns";
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	if(USActionComponent* OwnerComp = GetOwningComponent())
	{
		if(AActor* OwnerActor = OwnerComp->GetOwner())
		{
			USAttributeComponent* OwnerAttributeComp = USAttributeComponent::GetAttributes(OwnerActor);
			if(OwnerAttributeComp)
			{
				OwnerAttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
			}
		}
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	if(USActionComponent* OwnerComp = GetOwningComponent())
	{
		if(AActor* OwnerActor = OwnerComp->GetOwner())
		{
			USAttributeComponent* OwnerAttributeComp = USAttributeComponent::GetAttributes(OwnerActor);
			if(OwnerAttributeComp)
			{
				OwnerAttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
			}
		}
	}
	Super::StopAction_Implementation(Instigator);
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta < 0.0f)
	{
		if(AActor* Owner = OwningComp->GetOwner())
		{
			if(InstigatorActor == Owner)
				return;

			USAttributeComponent* OtherAttributes = USAttributeComponent::GetAttributes(InstigatorActor);
			if(OtherAttributes)
			{
				float OtherHealthDelta = FMath::RoundToInt(Delta * DamageMultiplier);
				OtherAttributes->ApplyHealthChange(Owner, OtherHealthDelta);
			}
		}
	}
}
