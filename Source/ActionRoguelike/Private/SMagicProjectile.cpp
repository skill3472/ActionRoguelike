// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	damage = 20;
	
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		if(USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, damage, SweepResult))
		{
			Explode();
		}
	}
}

