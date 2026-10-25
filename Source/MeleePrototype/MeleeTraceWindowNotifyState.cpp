// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeTraceWindowNotifyState.h"
#include "MeleePrototypeCharacter.h"
#include "MeleeCombatComponent.h"

void UMeleeTraceWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(
		MeshComp,
		Animation,
		TotalDuration,
		EventReference
	);
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[MeleeTraceWindow] Begin, Duration = %.3f"),
		TotalDuration
	);
}

void UMeleeTraceWindowNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(
		MeshComp,
		Animation,
		FrameDeltaTime,
		EventReference
	);

	if (!MeshComp)
		return;

	AActor* OwnerActor = MeshComp->GetOwner();
	UMeleeCombatComponent* CombatComponent = OwnerActor->FindComponentByClass<UMeleeCombatComponent>();

	if (!IsValid(CombatComponent))
		return;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[MeleeTraceWindow] Tick Owner=%s"),
		*GetNameSafe(OwnerActor)
	);

	CombatComponent->PerformMeleeTrace();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[MeleeTrace] Perform Owner=%s"),
		*GetNameSafe(OwnerActor)
	);
}

void UMeleeTraceWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(
		MeshComp,
		Animation,
		EventReference
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[MeleeTraceWindow] End")
	);

	if (!IsValid(MeshComp))
		return;

	AActor* OwnerActor = MeshComp->GetOwner();

	if (!IsValid(OwnerActor))
		return;

	UMeleeCombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UMeleeCombatComponent>();

	if (IsValid(CombatComponent))
		CombatComponent->EndAttack();
}
