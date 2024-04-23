// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interaction Component"), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteraction()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * 1000);
	
	// FHitResult hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);

	float sphereRadius = 30;
	TArray<FHitResult> hits;
	FCollisionShape shape;
	shape.SetSphere(sphereRadius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	for(FHitResult hit : hits)
	{
		AActor* hitActor = hit.GetActor();
		if(hitActor)
		{
			if(hitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				ISGameplayInterface::Execute_Interact(hitActor, MyPawn);
				if(bDebugDraw)
					DrawDebugSphere(GetWorld(), hit.ImpactPoint, sphereRadius, 32, LineColor, false, 2.0f);
				break;
			}
		}

	}

	if(bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}