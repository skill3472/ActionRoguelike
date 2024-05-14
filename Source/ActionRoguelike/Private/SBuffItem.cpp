// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuffItem.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASBuffItem::ASBuffItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	CreditsPrice = 0;
	cooldown = 10.0f;
	
	RepData.bIsEnabled = true;

	SetReplicates(true);
}

void ASBuffItem::Interact_Implementation(APawn* InstigatorPawn)
{
	RepData.PlayerState = Cast<ASCharacter>(InstigatorPawn)->GetPlayerState<ASPlayerState>();
	if(InstigatorPawn && RepData.bIsEnabled && RepData.PlayerState->HasCredits(CreditsPrice))
	{
		if(ApplyBuff(InstigatorPawn))
		{
			RepData.bIsEnabled = false;
			OnRep_BuffUsed();
		}
	}
}

void ASBuffItem::Cooldown_TimeElapsed()
{
	RepData.bIsEnabled = true;
	SetActorEnableCollision(RepData.bIsEnabled);
	RootComponent->SetVisibility(RepData.bIsEnabled, true);
}

bool ASBuffItem::ApplyBuff(APawn* InstigatorPawn)
{
	return false;
}

void ASBuffItem::OnRep_BuffUsed()
{
	RepData.PlayerState->ApplyCreditsChange(-CreditsPrice);
	SetActorEnableCollision(RepData.bIsEnabled);
	RootComponent->SetVisibility(RepData.bIsEnabled, true);

	FTimerHandle TimerHandle_BuffCooldown;
	GetWorldTimerManager().SetTimer(TimerHandle_BuffCooldown, this, &ASBuffItem::Cooldown_TimeElapsed, cooldown);
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

void ASBuffItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBuffItem, RepData);
}