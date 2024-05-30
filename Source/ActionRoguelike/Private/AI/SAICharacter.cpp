// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
 	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("pawnSensingComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("attributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("actionComp");
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	TimeToHitParamName = "TimeToHit";
	KillCredits = 20;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

int32 ASAICharacter::GetKillCredits() const
{
	return KillCredits;
}

void ASAICharacter::SetKillCredits(int32 KillCreditsToSet)
{
	KillCredits = KillCreditsToSet;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AActor* TargetActor = nullptr;
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		TargetActor = Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	}
	
	if(TargetActor && TargetActor != Pawn)
	{
		MulticastPawnSeen();
	}
	SetTargetActor(Pawn);
	// DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
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
		
		if(ActiveHealthBar == nullptr && NewHealth > 0.0f)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if(ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
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

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();
			
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

void ASAICharacter::MulticastPawnSeen_Implementation()
{
	USWorldUserWidget* CreatedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSeenWidgetClass);
	if(CreatedWidget)
	{
		CreatedWidget->AttachedActor = this;
		CreatedWidget->AddToViewport(10);
	}
}
