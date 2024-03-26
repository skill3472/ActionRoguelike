// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(RootComponent);
	ExplosionSphere = CreateDefaultSubobject<URadialForceComponent>("ExplosionSphere");
	ExplosionSphere->SetupAttachment(BaseMesh);
	ExplosionSphere->bImpulseVelChange = true;
	ExplosionSphere->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BaseMesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionSphere->FireImpulse();
	UE_LOG(LogTemp, Log, TEXT("OnActorHit reached (ExplosiveBarrel)"));
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

