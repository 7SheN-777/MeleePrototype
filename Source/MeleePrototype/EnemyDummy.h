#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyDummy.generated.h"

class UMeleeCombatComponent;
class UAnimMontage;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyAttackFinished,AEnemyDummy*,bool);

UCLASS()
class MELEEPROTOTYPE_API AEnemyDummy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyDummy();

    virtual float TakeDamage(
        float DamageAmount,
        FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool BeginMeleeAttack();

    UFUNCTION()
    void OnAttackMontageEnded(
        UAnimMontage* Montage,
        bool bInterrupted
    );

    FOnEnemyAttackFinished OnAttackFinished;

    bool IsDead() const { return bIsDead; }

    bool IsPlayingHitReact() const;
protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UMeleeCombatComponent> MeleeCombatComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> HitReactMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> DeathMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float CurrentHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> HealthBarWidget;

private:
    bool bIsDead = false;

    void Die();

    void OnDeathMontageEnded(
        UAnimMontage* Montage,
        bool bInterrupted
    );

    void UpdateHealthBar();
};