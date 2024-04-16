// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
 	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("pawnSensingComp");
	attributeComp = CreateDefaultSubobject<USAttributeComponent>("attributeComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TimeToHitParamName = "TimeToHit";
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	attributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);
	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(AAIController* AIC = Cast<AAIController>(GetController()))
	{
		bool bIsLow = USAttributeComponent::GetAttributes(this)->IsLowHealth();
		AIC->GetBlackboardComponent()->SetValueAsBool("IsLowHealth", bIsLow);
	}
	if(Delta < 0.0f)
	{
		// If damaged
		if(InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		if(NewHealth <= 0.0f)
		{
			// If killed
			// stop bt
			AAIController* AIC = Cast<AAIController>(GetController());
			if(AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed.");
			}
			
			// ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");
			
			// set lifespan
			SetLifeSpan(10.0f);
		}
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);;
	}
}

