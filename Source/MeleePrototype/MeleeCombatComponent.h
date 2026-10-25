// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeAttackConfig.h"
#include "Containers/Set.h"
#include "MeleeCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEEPROTOTYPE_API UMeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeleeCombatComponent();

protected:

public:	


public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Melee")
	FMeleeAttackConfig AttackConfig;

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void PerformMeleeTrace();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void ResetHitActors();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void BeginAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndAttack();
private:
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	bool bIsAttacking = false;
};
