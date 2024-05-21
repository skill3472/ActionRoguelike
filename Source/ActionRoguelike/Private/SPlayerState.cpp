// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Net/UnrealNetwork.h"

// void ASPlayerState::MulticastCreditsChanged_Implementation(APlayerState* PlayerState, int Delta, int NewCredits)
// {
// 	OnCreditsChanged.Broadcast(PlayerState, Delta, NewCredits);
// }


ASPlayerState::ASPlayerState()
{
	PlayerCredits = 0;
}

bool ASPlayerState::ApplyCreditsChange(int Delta)
{
	if(PlayerCredits + Delta < 0)
		return false;
	
	int32 OldCredits = PlayerCredits;
	PlayerCredits += Delta;

	FString Msg = FString::Printf(TEXT("Credits changed from %d to %d"), OldCredits, PlayerCredits);
	LogOnScreen(this, Msg);
	
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

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		SaveObject->Credits = PlayerCredits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		if(SaveObject->Credits >= 0)
			ApplyCreditsChange(SaveObject->Credits);	
	}
}


void ASPlayerState::OnRep_PlayerCredits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, PlayerCredits - OldCredits, PlayerCredits);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, PlayerCredits);
}
