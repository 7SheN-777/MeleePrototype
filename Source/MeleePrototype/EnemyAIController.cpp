// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "EnemyDummy.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
		return;

	AEnemyDummy* Enemy = Cast<AEnemyDummy>(ControlledPawn);

	if (!IsValid(Enemy))
		return;

	if (Enemy->IsDead())
	{
		StopMovement();
		return;
	}

	if (CurrentState == EEnemyAIState::Attacking)
	{
		if (!bNeedsAIReevaluation)
			return;
		if (Enemy->IsPlayingHitReact())
			return;
		bNeedsAIReevaluation = false;
		CurrentState = EEnemyAIState::Idle;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!IsValid(PlayerCharacter))
	{
		if (CurrentState != EEnemyAIState::Idle)
		{
			StopMovement();
			CurrentState = EEnemyAIState::Idle;

			UE_LOG(LogTemp, Log, TEXT("EnemyAI: Idle"));
		}
		return;
	}

	const float Distance = FVector::Dist2D(ControlledPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	EEnemyAIState DesiredState;

	if (Distance > DetectionRadius)
	{
		DesiredState = EEnemyAIState::Idle;
	}
	else if (Distance <= StopDistance)
	{
		DesiredState = EEnemyAIState::ReadyToAttack;
	}
	else
	{
		DesiredState = EEnemyAIState::Chasing;
	}

	if (DesiredState != CurrentState)
	{
		CurrentState = DesiredState;

		switch (CurrentState)
		{
		case EEnemyAIState::Idle:
			StopMovement();
			UE_LOG(LogTemp, Log, TEXT("EnemyAI: Idle"));
			break;
		case EEnemyAIState::Chasing:
			UE_LOG(LogTemp, Log, TEXT("EnemyAI: Chasing"));

			StartChasing(PlayerCharacter);
			break;
		case EEnemyAIState::ReadyToAttack:
			StopMovement();

			UE_LOG(
				LogTemp,
				Log,
				TEXT("EnemyAI: ReadyToAttack")
			);

			if (Enemy->BeginMeleeAttack())
			{
				CurrentState = EEnemyAIState::Attacking;
				UE_LOG(
					LogTemp,
					Log,
					TEXT("EnemyAI: Attacking")
				);
			}

			break;
		case EEnemyAIState::Attacking:
			
			break;
		default:
			break;
		}
	}

	if (CurrentState == EEnemyAIState::Chasing && GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		StartChasing(PlayerCharacter);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[EnemyAI] Possessed Pawn: %s"),
		*GetNameSafe(InPawn)
	);

	AEnemyDummy* Enemy = Cast<AEnemyDummy>(InPawn);

	if (IsValid(Enemy))
	{
		BoundEnemy = Enemy;

		AttackFinishedDelegateHandle = Enemy->OnAttackFinished.AddUObject(this, &AEnemyAIController::HandleEnemyAttackFinished);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[EnemyAI] Bound attack finished event.")
		);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[EnemyAI] Possessed Pawn: %s"),
		IsValid(InPawn) ? *InPawn->GetName() : TEXT("None")
	);
}

void AEnemyAIController::OnUnPossess()
{
	if (AEnemyDummy* Enemy = BoundEnemy.Get())
	{
		if (AttackFinishedDelegateHandle.IsValid())
		{
			Enemy->OnAttackFinished.Remove(AttackFinishedDelegateHandle);
		}
	}

	BoundEnemy.Reset();
	AttackFinishedDelegateHandle.Reset();

	Super::OnUnPossess();
}

void AEnemyAIController::StartChasing(ACharacter* PlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
		return;

	const EPathFollowingRequestResult::Type Result = MoveToActor(PlayerCharacter, 0.0f, true, true, false, nullptr, true);

	if(Result == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EnemyAI: Failed to start chasing player.")
		);
	}
}

void AEnemyAIController::HandleEnemyAttackFinished(AEnemyDummy* Enemy, bool bInterrupted)
{
	if (Enemy != BoundEnemy.Get())
		return;
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EnemyAI] Attack finished. Interrupted: %s"),
		bInterrupted ? TEXT("true") : TEXT("false")
	);

	bNeedsAIReevaluation = true;
}
