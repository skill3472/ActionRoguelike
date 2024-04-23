// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRoguelike/Public/SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
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

	springArmComp->bUsePawnControlRotation  = true;
	bUseControllerRotationYaw = false;

	interactionComp = CreateDefaultSubobject<USInteractionComponent>("interactionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	HandSocketName = "Muzzle_01";
	TimeToHitParamName = "TimeToHit";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	attributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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
	SpawnProjectile(projectileClass);
}

void ASCharacter::SecondaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_TimeElapsed, attackDelay);
}

void ASCharacter::SecondaryAttack_TimeElapsed()
{
	SpawnProjectile(secondaryProjectileClass);
}

void ASCharacter::TeleportAbility()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportAbility, this, &ASCharacter::TeleportAbility_TimeElapsed, attackDelay);
}

void ASCharacter::TeleportAbility_TimeElapsed()
{
	SpawnProjectile(teleportProjectileClass);
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
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &ASCharacter::TeleportAbility);
	
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteraction);

}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> projectileToSpawn)
{
	FVector handLocation = GetMesh()->GetSocketLocation(HandSocketName);
	UGameplayStatics::SpawnEmitterAttached(muzzleFlash, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

	FHitResult hit;
	FRotator projectileRotation;
	APlayerCameraManager *camManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector start = camManager->GetCameraLocation();
	FVector camForward  = camManager->GetCameraRotation().Vector();
	FVector end = start + (camForward * 5000);
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams QueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	QueryParams.AddIgnoredActor(this);
	
	bool bHitTarget = GetWorld()->LineTraceSingleByObjectType(hit, start, end, ObjectQueryParams,  QueryParams);
	FVector hitPoint = hit.ImpactPoint;
	if(bHitTarget)
	{
		end = hit.ImpactPoint;
	}
	projectileRotation = UKismetMathLibrary::FindLookAtRotation(handLocation, end);
	if(CVarDebugDrawAim.GetValueOnGameThread())
		DrawDebugLine(GetWorld(), start, end, FColor::Purple, false, 2.0f, 0, 2.0f);
	
	FTransform spawnTM = FTransform(projectileRotation, handLocation);
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Instigator = this;
	
	GetWorld()->SpawnActor<AActor>(projectileToSpawn, spawnTM, spawnParams);
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

