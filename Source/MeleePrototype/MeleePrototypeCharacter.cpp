// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleePrototypeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MeleePrototype.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "MeleeCombatComponent.h"

AMeleePrototypeCharacter::AMeleePrototypeCharacter()
{
	MeleeCombatComponent = CreateDefaultSubobject<UMeleeCombatComponent>(TEXT("MeleeCombatComponent"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMeleePrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMeleePrototypeCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMeleePrototypeCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMeleePrototypeCharacter::Look);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMeleePrototypeCharacter::Attack);
	}
	else
	{
		UE_LOG(LogMeleePrototype, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMeleePrototypeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMeleePrototypeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMeleePrototypeCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMeleePrototypeCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMeleePrototypeCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMeleePrototypeCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

float AMeleePrototypeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.0f || CurrentHealth <= 0.0f)
		return 0.0f;

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
		*GetName(),
		ActualDamage,
		CurrentHealth,
		MaxHealth
	);

	USkeletalMeshComponent* CharacterMesh = GetMesh();

	if (CurrentHealth <= 0.0f)
	{
		SetActorEnableCollision(false);
		SetLifeSpan(1.0f);
	}
	else if (IsValid(HitReactMontage) && IsValid(CharacterMesh))
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(HitReactMontage);
			UE_LOG(LogTemp, Warning, TEXT("[Player] Hit React Montage Length: %.3f"), MontageLength);
		}
	}
	return ActualDamage;
}

void AMeleePrototypeCharacter::Attack()
{
	if (MeleeCombatComponent)
		MeleeCombatComponent->BeginAttack();

	if (bIsAttacking || !AttackMontage)
		return;

	USkeletalMeshComponent* CharacterMesh = GetMesh();

	if (!CharacterMesh)
		return;

	UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();

	if(!AnimInstance)
		return;

	const float Duration = AnimInstance->Montage_Play(AttackMontage);

	if (Duration <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to play attack montage"));
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AMeleePrototypeCharacter::OnAttackMontageEnded);

	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);


	bIsAttacking = true;

	UE_LOG(LogTemp, Log, TEXT("Attack action triggered"));
}

void AMeleePrototypeCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage)
		return;
	bIsAttacking = false;

	UE_LOG(LogTemp, Log, TEXT("Attack montage ended. Interrupted: %s"), bInterrupted ? TEXT("true") : TEXT("false"));
}