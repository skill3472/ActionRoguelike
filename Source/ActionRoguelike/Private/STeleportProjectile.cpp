// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Kismet/GameplayStatics.h"

ASTeleportProjectile::ASTeleportProjectile()
{
	sphereComp->OnComponentHit.AddDynamic(this, &ASTeleportProjectile::OnActorHit);
	movementComp->InitialSpeed = 6000.0f;
}

void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ASTeleportProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ASTeleportProjectile::BeginPlay()
{
	sphereComp->IgnoreActorWhenMoving(Cast<AActor>(this)->GetInstigator(), true);
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &ASTeleportProjectile::Explode, 0.2f);
	Super::BeginPlay();
}

void ASTeleportProjectile::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, explosionEffect, GetActorLocation(), GetActorRotation());
	movementComp->SetVelocityInLocalSpace(FVector(0,0,0));
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::Teleport, 0.2f);
}

void ASTeleportProjectile::Teleport()
{
	APawn* Player = Cast<AActor>(this)->GetInstigator();
	Player->TeleportTo(GetActorLocation(), GetActorRotation(), false, false);
	Destroy();
}



