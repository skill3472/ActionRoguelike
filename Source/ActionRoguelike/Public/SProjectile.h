// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "SProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* sphereComp;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* movementComp;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* effectsComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audio")
	UAudioComponent* audioComp;

	UPROPERTY(EditAnywhere, Category="VFX")
	UParticleSystem* explosionEffect;

	UPROPERTY(EditAnywhere, Category="Audio")
	USoundCue* explodeSound;

	UFUNCTION()
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

};
