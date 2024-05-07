// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuffItem.h"

// Sets default values
ASBuffItem::ASBuffItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	CreditsPrice = 0;
	cooldown = 10.0f;
	
	bIsEnabled = true;

	SetReplicates(true);
}

void ASBuffItem::Interact_Implementation(APawn* InstigatorPawn)
{
	ASPlayerState* PlayerState = Cast<ASCharacter>(InstigatorPawn)->GetPlayerState<ASPlayerState>();
	if(InstigatorPawn && bIsEnabled && PlayerState->HasCredits(CreditsPrice))
	{
		if(ApplyBuff(InstigatorPawn))
		{
			PlayerState->ApplyCreditsChange(-CreditsPrice);
			bIsEnabled = false;
			SetActorEnableCollision(bIsEnabled);
			RootComponent->SetVisibility(bIsEnabled, true);

			FTimerHandle TimerHandle_BuffCooldown;
			GetWorldTimerManager().SetTimer(TimerHandle_BuffCooldown, this, &ASBuffItem::Cooldown_TimeElapsed, cooldown);
		}
	}
}

void ASBuffItem::Cooldown_TimeElapsed()
{
	bIsEnabled = true;
	SetActorEnableCollision(bIsEnabled);
	RootComponent->SetVisibility(bIsEnabled, true);
}

bool ASBuffItem::ApplyBuff(APawn* InstigatorPawn)
{
	return false;
}

// Called when the game starts or when spawned
void ASBuffItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBuffItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

