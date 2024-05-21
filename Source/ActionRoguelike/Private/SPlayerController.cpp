// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"

void ASPlayerController::TogglePauseMenu()
{
	if(PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
	else
	{
		PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
		if(PauseMenuInstance)
		{
			PauseMenuInstance->AddToViewport(100);

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}	
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
}

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlueprintBeginPlayingState();
}

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	OnPlayerChanged.Broadcast(InPawn);
}
