// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Kismet/GameplayStatics.h"

ASTeleportProjectile::ASTeleportProjectile()
{
	
	movementComp->InitialSpeed = 6000.0f;
}

void ASTeleportProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ASTeleportProjectile::BeginPlay()
{
	sphereComp->IgnoreActorWhenMoving(Cast<AActor>(this)->GetInstigator(), true);
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &ASTeleportProjectile::Explode, 0.2f);
	Super::BeginPlay();
}

void ASTeleportProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Explode);
	UGameplayStatics::SpawnEmitterAtLocation(this, explosionEffect, GetActorLocation(), GetActorRotation());
	effectsComp->DeactivateSystem();
	movementComp->StopMovementImmediately();
	SetActorEnableCollision(false);
	FTimerHandle TimerHandle_Teleport;
	
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASTeleportProjectile::Teleport, 0.2f);
}

void ASTeleportProjectile::Teleport()
{
	AActor* Player = GetInstigator();
	if(ensure(Player))
	{
		Player->TeleportTo(GetActorLocation(), Player->GetActorRotation(), false, false);
	}
	Destroy();
}



