// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "SAttributeComponent.h"



USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if(ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if(MyPawn == nullptr)
			return EBTNodeResult::Failed;
		
		const FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");

		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if(TargetActor == nullptr)
			return EBTNodeResult::Failed; // temporarily

		if(!USAttributeComponent::IsActorAlive(TargetActor))
			return EBTNodeResult::Failed;
		
		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator Rotation = Direction.Rotation();

		// random rotation offset, to fix aimbot xD
		Rotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		Rotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyPawn;

		const AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, Rotation);

		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}


