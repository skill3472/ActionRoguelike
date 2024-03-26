// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"

// Sets default values
ASProjectile::ASProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>("sphereComp");
	sphereComp->SetCollisionProfileName("Projectile");
	RootComponent = sphereComp;

	effectsComp = CreateDefaultSubobject<UParticleSystemComponent>("effectsComp");
	effectsComp->SetupAttachment(sphereComp);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>("movementComp");
	movementComp->InitialSpeed = 1000.0f;
	movementComp->bRotationFollowsVelocity = true;
	movementComp->bInitialVelocityInLocalSpace = true;
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

