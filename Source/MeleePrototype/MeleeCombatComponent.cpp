// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCombatComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UMeleeCombatComponent::UMeleeCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UMeleeCombatComponent::PerformMeleeTrace()
{
	AActor* OwnerActor = GetOwner();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[CombatState] Owner=%s, IsAttacking=%s"),
		*GetNameSafe(OwnerActor),
		bIsAttacking ? TEXT("true") : TEXT("false")
	);


	if (!IsValid(OwnerActor) || !bIsAttacking)
		return;

	const FVector Start = OwnerActor->GetActorTransform().TransformPosition(AttackConfig.TraceStartOffset);

	const FVector End = Start + OwnerActor->GetActorForwardVector() * AttackConfig.TraceDistance;

	const float TraceRadius = AttackConfig.TraceRadius;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PerformMeleeTrace), false, OwnerActor);

	FHitResult HitResult;

	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[MeleeTrace] Hit=%s, Actor=%s"),
		bHit ? TEXT("true") : TEXT("false"),
		bHit && HitResult.GetActor()
		? *HitResult.GetActor()->GetName()
		: TEXT("None")
	);
	
	DrawDebugSphere(GetWorld(), Start, TraceRadius, 12, FColor::Green, false, 2.0f);
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Yellow,
		false,
		5.0f,
		0,
		4.0f
	);
	DrawDebugSphere(GetWorld(), End, TraceRadius, 12, bHit ? FColor::Red : FColor::Green, false, 2.0f);
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) && HitActor != OwnerActor)
		{
			if (HitActors.Contains(HitActor))
			{
				UE_LOG(
					LogTemp,
					Verbose,
					TEXT("Already hit this actor in the current attack: %s"),
					*GetNameSafe(HitActor)
				);

				return;
			}
			HitActors.Add(HitActor);
			const float AppliedDamage = UGameplayStatics::ApplyDamage(
				HitActor,
				AttackConfig.Damage,
				OwnerActor->GetInstigatorController(),
				OwnerActor,
				UDamageType::StaticClass()
			);
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Applied %.1f damage to %s"),
				AppliedDamage,
				*GetNameSafe(HitActor)
			);

			UE_LOG(
				LogTemp,
				Log,
				TEXT("First hit in this attack: %s"),
				*GetNameSafe(HitActor)
			);

			DrawDebugSphere(
				GetWorld(),
				HitResult.ImpactPoint,
				10.0f,
				12,
				FColor::Red,
				false,
				2.0f
			);
		}
	}
}

void UMeleeCombatComponent::ResetHitActors()
{
	HitActors.Reset();
}

void UMeleeCombatComponent::BeginAttack()
{
	ResetHitActors();
	bIsAttacking = true;
}

void UMeleeCombatComponent::EndAttack()
{
	bIsAttacking = false;
}
