// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"

#include "SpartaGameState.h"
#include "SpartaPlayerController.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Tags.Add(TEXT("Player"));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (MannyMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MannyMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(
			FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> MannyAnim(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C"));
	if (MannyAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MannyAnim.Class);
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.6f;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	bReverseControl = false;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (PC->MoveAction)
	{
		EnhancedInput->BindAction(
			PC->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
	}

	if (PC->JumpAction)
	{
		EnhancedInput->BindAction(
			PC->JumpAction, ETriggerEvent::Started, this, &ASpartaCharacter::StartJump);
		EnhancedInput->BindAction(
			PC->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
	}

	if (PC->LookAction)
	{
		EnhancedInput->BindAction(
			PC->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
	}

	if (PC->SprintAction)
	{
		EnhancedInput->BindAction(
			PC->SprintAction, ETriggerEvent::Started, this, &ASpartaCharacter::StartSprint);
		EnhancedInput->BindAction(
			PC->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
	}
}

void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

	FVector2D MoveInput = Value.Get<FVector2D>();

	if (bReverseControl)
	{
		MoveInput *= -1.0f;
	}

	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(Forward, MoveInput.Y);
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(Right, MoveInput.X);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(-LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NormalSpeed * SprintSpeedMultiplier;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage =
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	if (UWorld* World = GetWorld())
	{
		if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
		{
			GameState->OnGameOver();
		}
	}
}