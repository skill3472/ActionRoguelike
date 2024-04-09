// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuffItem.h"

// Sets default values
ASBuffItem::ASBuffItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	
	bIsEnabled = true;
}

void ASBuffItem::Interact_Implementation(APawn* InstigatorPawn)
{
	if(InstigatorPawn && bIsEnabled)
	{
		bIsEnabled = false;
		SetActorEnableCollision(bIsEnabled);
		RootComponent->SetVisibility(bIsEnabled, true);
		GetWorldTimerManager().SetTimer(TimerHandle_BuffCooldown, this, &ASBuffItem::Cooldown_TimeElapsed, cooldown);
	}
}

void ASBuffItem::Cooldown_TimeElapsed()
{
	bIsEnabled = true;
	SetActorEnableCollision(bIsEnabled);
	RootComponent->SetVisibility(bIsEnabled, true);
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

