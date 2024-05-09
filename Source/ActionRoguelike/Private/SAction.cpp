// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"
#include "Net/UnrealNetwork.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Starting action: %s"), *GetNameSafe(this));

	USActionComponent* OwnerComp = GetOwningComponent();
	OwnerComp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping action: %s"), *GetNameSafe(this));

	// ensureAlways(bIsRunning);
	
	USActionComponent* OwnerComp = GetOwningComponent();
	OwnerComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if(IsRunning())
		return false;
	
	USActionComponent* OwnerComp = GetOwningComponent();

	if(OwnerComp->ActiveGameplayTags.HasAny(BlockedTags))
		return false;

	return true;
}

USAction::USAction()
{
	bAutoStart = false;
}

UWorld* USAction::GetWorld() const
{
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

void USAction::OnRep_IsRunning()
{
	if(bIsRunning)
	{
		StartAction(nullptr);
	}
	else
	{
		StopAction(nullptr);
	}
}

bool USAction::IsRunning() const
{
	return bIsRunning;
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, bIsRunning);
}
