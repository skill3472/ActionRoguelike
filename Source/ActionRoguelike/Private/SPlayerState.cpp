// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

bool ASPlayerState::ApplyCreditsChange(int Delta)
{
	if(PlayerCredits + Delta < 0)
		return false;

	PlayerCredits += Delta;
	OnCreditsChanged.Broadcast(this, Delta, PlayerCredits);
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