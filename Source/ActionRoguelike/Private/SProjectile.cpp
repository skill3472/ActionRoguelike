// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"

#include "SAttributeComponent.h"
#include "CADKernel/Utils/StringUtil.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectile::ASProjectile()
{
	sphereComp = CreateDefaultSubobject<USphereComponent>("sphereComp");
	sphereComp->SetCollisionProfileName("Projectile");
	sphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASProjectile::OnActorOverlap);
	sphereComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnActorHit);
	RootComponent = sphereComp;

	effectsComp = CreateDefaultSubobject<UParticleSystemComponent>("effectsComp");
	effectsComp->SetupAttachment(sphereComp);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>("movementComp");
	movementComp->InitialSpeed = 1000.0f;
	movementComp->bRotationFollowsVelocity = true;
	movementComp->bInitialVelocityInLocalSpace = true;
	movementComp->ProjectileGravityScale = 0.0f;

	audioComp = CreateDefaultSubobject<UAudioComponent>("audioComp");
	audioComp->SetupAttachment(sphereComp);

	//SetReplicates(true);
}

void ASProjectile::PostInitializeComponents()
{
	sphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	Super::PostInitializeComponents();
}

void ASProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USAttributeComponent* attributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(attributeComp)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Cast<USoundBase>(explodeSound), GetActorLocation());
			attributeComp->ApplyHealthChange(GetInstigator(), -damage);
			Destroy();
		}
	}
}

void ASProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ASProjectile::Explode_Implementation()
{
	if(ensure(IsValid(this)))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Cast<USoundBase>(explodeSound), GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(this, explosionEffect, GetActorLocation(), GetActorRotation());
		Destroy();
	}
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	audioComp->Play();
}

