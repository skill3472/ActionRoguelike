// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier to Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	maxHealth = 100;
	Health = maxHealth;
	LowHealthThreshold = 50.0f;
	MaxRage = 100.0f;
	Rage = 0.0f;
	DamageToRageMultiplier = 5.0f;

	bUsesRage = false;
}


bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
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
	if(!GetOwner()->CanBeDamaged() && delta < 0.0f)
		return false;

	if(delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		delta *= DamageMultiplier;

		ApplyRageChange(InstigatorActor, (-delta)*DamageToRageMultiplier);
	}
	
	float oldHealth = Health;

	Health = FMath::Clamp(Health + delta, 0.0f, maxHealth);
	
	float ActualDelta = Health - oldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	// Died
	if(ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if(ensure(GM))
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	
	return ActualDelta != 0;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	if(InstigatorActor == GetOwner() && Delta > 0.0f)
		return false;
	if(bUsesRage)
	{
		float OldRage = Rage;
		Rage = FMath::Clamp(Rage + Delta, 0.0f, MaxRage);
		float ActualDelta = Rage - OldRage;
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
		
		return ActualDelta != 0;
	}
	return false;
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

void USAttributeComponent::UseRage(bool UseRage)
{
	bUsesRage = UseRage;
}

bool USAttributeComponent::UsesRage()
{
	return bUsesRage;
}

float USAttributeComponent::GetRage()
{
	return Rage;
}

