// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<bool> CVarDebugDrawAim(TEXT("su.AimDebugDraw"), false, TEXT("Enable Debug Lines for Attack Aiming"), ECVF_Cheat);

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springArmComp = CreateDefaultSubobject<USpringArmComponent>("springArmComp");
	springArmComp->SetupAttachment(RootComponent);
	
	cameraComp = CreateDefaultSubobject<UCameraComponent>("cameraComp");
	cameraComp->SetupAttachment(springArmComp);

	attributeComp = CreateDefaultSubobject<USAttributeComponent>("attributeComp");
	attributeComp->UseRage(true);

	ActionComp = CreateDefaultSubobject<USActionComponent>("actionComp");

	springArmComp->bUsePawnControlRotation  = true;
	bUseControllerRotationYaw = false;

	interactionComp = CreateDefaultSubobject<USInteractionComponent>("interactionComp");

	MyController = Cast<ASPlayerController>(GetController());

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	attributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	if(MyController)
		MyController->OnPlayerChanged.AddDynamic(this, &ASCharacter::OnPlayerChanged);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return cameraComp->GetComponentLocation();
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;
	
	AddMovementInput(ControlBot.Vector(), value);
}

void ASCharacter::MoveRight(float value)
{
	FRotator ControlBot = GetControlRotation();
	ControlBot.Pitch = 0.0f;
	ControlBot.Roll = 0.0f;
	
	FVector RightVector = FRotationMatrix(ControlBot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, value);
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::Jump()
{
	Super::Jump();
}

void ASCharacter::PrimaryInteraction()
{
	interactionComp->PrimaryInteraction();
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::SecondaryAttack()
{
	ActionComp->StartActionByName(this, "SecondaryAttack");
}

void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &ASCharacter::Dash);
	
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteraction);

}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}
	if(NewHealth <= 0.0f && Delta < 0.0f) // Died
	{
		APlayerController* pc = Cast<APlayerController>(GetController());
		DisableInput(pc);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
	}
}

void ASCharacter::OnPlayerChanged(APawn* MyPawn)
{
	Destroy(); // @fixme something doesn't work here, the character is not destroyed
}

/*
*	Cheats
*/
void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	attributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::GiveCredits(int Delta /* = 1000 */)
{
	GetPlayerState<ASPlayerState>()->ApplyCreditsChange(Delta);
}

