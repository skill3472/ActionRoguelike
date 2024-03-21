// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SMagicProjectile.generated.h"

class UParticleSystemComponent;

UCLASS()
class ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* sphereComp;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* movementComp;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* effectsComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
