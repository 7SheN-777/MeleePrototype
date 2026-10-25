// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeTraceNotify.h"
#include "MeleePrototypeCharacter.h"

void UMeleeTraceNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;
	AActor* Owner = MeshComp->GetOwner();
	if(!Owner)
		return;
	AMeleePrototypeCharacter* Character = Cast<AMeleePrototypeCharacter>(Owner);
	if(!Character)
		return;
}