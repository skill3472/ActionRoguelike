// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interaction Component"), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
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

	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
	
	// FHitResult hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);
	
	TArray<FHitResult> hits;
	FCollisionShape shape;
	shape.SetSphere(TraceRadius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before query
	FocusedActor = nullptr;
	
	for(FHitResult hit : hits)
	{
		if(bDebugDraw)
			DrawDebugSphere(GetWorld(), hit.ImpactPoint, TraceRadius, 32, LineColor, false, 2.0f);
		
		AActor* hitActor = hit.GetActor();
		if(hitActor)
		{
			if(hitActor->Implements<USGameplayInterface>())
			{
				FocusedActor = hitActor;
				
				break;
			}
		}

	}

	if(FocusedActor)
	{
		if(DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;
			if(!DefaultWidgetInstance->IsInViewport())
				DefaultWidgetInstance->AddToViewport();
		}
	}
	else
	{
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	
	if(bDebugDraw)
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
}


void USInteractionComponent::PrimaryInteraction()
{
	if(FocusedActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "No focus actor to interact.");
		return;
	}
	
	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(FocusedActor, MyPawn);
}