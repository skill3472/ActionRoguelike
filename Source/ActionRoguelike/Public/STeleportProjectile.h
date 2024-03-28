// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectile.h"
#include "STeleportProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASTeleportProjectile : public ASProjectile
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	UParticleSystem* explosionEffect;
	
	virtual void BeginPlay() override;
	FTimerHandle TimerHandle_Explode;
	FTimerHandle TimerHandle_Teleport;
	void Explode();
	void Teleport();
	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	ASTeleportProjectile();
};
