// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	healAmount = 30;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* attributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if(attributeComp)
	{
		if(attributeComp->ApplyHealthChange(healAmount) && bIsEnabled)
		{
			bIsEnabled = false;
			SetActorEnableCollision(bIsEnabled);
			RootComponent->SetVisibility(bIsEnabled, true);
			GetWorldTimerManager().SetTimer(TimerHandle_BuffCooldown, this, &ASHealthPotion::Cooldown_TimeElapsed, cooldown);
		}
	}
}

// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

