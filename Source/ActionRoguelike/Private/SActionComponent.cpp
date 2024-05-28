// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}




void USActionComponent::BeginPlay()
{
	Super::BeginPlay();


	if(GetOwner()->HasAuthority()) // SERVER ONLY
	{
		for(TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}




void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FString Msg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Msg);

	for(USAction* Action : Actions)
	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if(!ensure(ActionClass))
		return;

	// Skip for clients
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempted to add action. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	
	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);

		if(NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
	
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if(!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
		return;

	Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionName)
		{
			if(!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			if(!GetOwner()->HasAuthority()) // is client
				ServerStartAction(Instigator, ActionName);
			
			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->ActionName == ActionName)
		{
			if(Action->IsRunning())
			{
				if(!GetOwner()->HasAuthority()) // is client
					ServerStopAction(Instigator, ActionName);
				
				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

bool USActionComponent::HasAction(TSubclassOf<USAction> ActionToCheck)
{
	for(USAction* Action : Actions)
	{
		if(Action && Action->GetClass() == ActionToCheck)
		{
			return true;
		}
	}
	return false;
}

USActionComponent* USActionComponent::GetActionComp(AActor* FromActor)
{
	if(FromActor)
	{
		return FromActor->FindComponentByClass<USActionComponent>();
	}
	return nullptr;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}


bool USActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for(USAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}
