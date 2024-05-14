// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

void ASPlayerState::MulticastCreditsChanged_Implementation(APlayerState* PlayerState, int Delta, int NewCredits)
{
	OnCreditsChanged.Broadcast(PlayerState, Delta, NewCredits);
}

bool ASPlayerState::ApplyCreditsChange(int Delta)
{
	if(PlayerCredits + Delta < 0)
		return false;

	PlayerCredits += Delta;
	if(Delta != 0.0f)
		MulticastCreditsChanged(this, Delta, PlayerCredits);
	return true;
}

bool ASPlayerState::HasCredits(int Amount)
{
	if(PlayerCredits < Amount)
		return false;
	return true;
}

int32 ASPlayerState::GetCredits()
{
	return PlayerCredits;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, PlayerCredits);
}