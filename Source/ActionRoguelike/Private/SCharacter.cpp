// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	springArmComp->bUsePawnControlRotation  = true;
	bUseControllerRotationYaw = false;

	interactionComp = CreateDefaultSubobject<USInteractionComponent>("interactionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

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
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, attackDelay);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	FVector handLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	FHitResult hit;
	FRotator projectileRotation;
	APlayerCameraManager *camManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector start = camManager->GetCameraLocation();
	FVector camForward  = camManager->GetCameraRotation().Vector();
	FVector end = start + (camForward * 15000);
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams QueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	bool bHitTarget = GetWorld()->LineTraceSingleByObjectType(hit, start, end, ObjectQueryParams,  QueryParams);
	FVector hitPoint = hit.ImpactPoint;
	if(bHitTarget)
	{
		projectileRotation = UKismetMathLibrary::FindLookAtRotation(handLocation, hitPoint);
		DrawDebugLine(GetWorld(), start, hit.Location, FColor::Purple, false, 2.0f, 0, 2.0f);
	}
	else
	{
		projectileRotation = UKismetMathLibrary::FindLookAtRotation(handLocation, end);
		DrawDebugLine(GetWorld(), start, end, FColor::Purple, false, 2.0f, 0, 2.0f);
	}
	
	FTransform spawnTM = FTransform(projectileRotation, handLocation); // New rotation code here?
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Instigator = this;
	
	GetWorld()->SpawnActor<AActor>(projectileClass, spawnTM, spawnParams);
}

void ASCharacter::SecondaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_TimeElapsed, attackDelay);
}

void ASCharacter::SecondaryAttack_TimeElapsed()
{
	FVector handLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	FHitResult hit;
	FRotator projectileRotation;
	APlayerCameraManager *camManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector start = camManager->GetCameraLocation();
	FVector camForward  = camManager->GetCameraRotation().Vector();
	FVector end = start + (camForward * 15000);
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams QueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	bool bHitTarget = GetWorld()->LineTraceSingleByObjectType(hit, start, end, ObjectQueryParams,  QueryParams);
	FVector hitPoint = hit.ImpactPoint;
	if(bHitTarget)
	{
		projectileRotation = UKismetMathLibrary::FindLookAtRotation(handLocation, hitPoint);
		DrawDebugLine(GetWorld(), start, hit.Location, FColor::Purple, false, 2.0f, 0, 2.0f);
	}
	else
	{
		projectileRotation = UKismetMathLibrary::FindLookAtRotation(handLocation, end);
		DrawDebugLine(GetWorld(), start, end, FColor::Purple, false, 2.0f, 0, 2.0f);
	}
	
	FTransform spawnTM = FTransform(projectileRotation, handLocation); // New rotation code here?
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Instigator = this;
	
	GetWorld()->SpawnActor<AActor>(secondaryProjectileClass, spawnTM, spawnParams);
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
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteraction);

}

