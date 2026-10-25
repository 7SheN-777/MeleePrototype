#include "EnemyDummy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

#include "EnemyHealthBarWidget.h"
#include "MeleeCombatComponent.h"
#include "EnemyAIController.h"

// Sets default values
AEnemyDummy::AEnemyDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 创建近战战斗组件
    MeleeCombatComponent =
        CreateDefaultSubobject<UMeleeCombatComponent>(
            TEXT("MeleeCombatComponent")
        );

    // 使用 ACharacter 自带的胶囊体
    GetCapsuleComponent()->InitCapsuleSize(50.0f, 70.0f);

    GetCapsuleComponent()->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    // 使用 ACharacter 自带的骨骼网格
    GetMesh()->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );

    // 配置基础角色移动
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 540.0f, 0.0f);

    GetCharacterMovement()->MaxWalkSpeed = 300.0f;

    // 创建血条组件
    HealthBarWidget =
        CreateDefaultSubobject<UWidgetComponent>(
            TEXT("HealthBarWidget")
        );

    HealthBarWidget->SetupAttachment(GetRootComponent());
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HealthBarWidget->SetDrawSize(FVector2D(150.0f, 20.0f));
    HealthBarWidget->SetRelativeLocation(
        FVector(0.0f, 0.0f, 120.0f)
    );
    HealthBarWidget->SetTwoSided(true);
}

bool AEnemyDummy::IsPlayingHitReact() const
{
    if (!IsValid(HitReactMontage))
    {
        return false;
    }

    const USkeletalMeshComponent* CharacterMesh = GetMesh();

    if (!IsValid(CharacterMesh))
    {
        return false;
    }

    const UAnimInstance* AnimInstance =
        CharacterMesh->GetAnimInstance();

    return IsValid(AnimInstance) &&
        AnimInstance->Montage_IsPlaying(HitReactMontage);
}

void AEnemyDummy::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    UpdateHealthBar();
}

float AEnemyDummy::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (bIsDead)
    {
        return 0.0f;
    }

    const float ActualDamage = Super::TakeDamage(
        DamageAmount,
        DamageEvent,
        EventInstigator,
        DamageCauser
    );

    if (ActualDamage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return 0.0f;
    }

    CurrentHealth = FMath::Clamp(
        CurrentHealth - ActualDamage,
        0.0f,
        MaxHealth
    );

    UpdateHealthBar();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
        *GetName(),
        ActualDamage,
        CurrentHealth,
        MaxHealth
    );

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
    else if (IsValid(HitReactMontage))
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            // 受击后结束当前攻击判定
            if (IsValid(MeleeCombatComponent))
            {
                MeleeCombatComponent->EndAttack();
            }

            // 停止正在播放的攻击 Montage
            if (IsValid(AttackMontage))
            {
                AnimInstance->Montage_Stop(0.1f, AttackMontage);
            }

            // 播放受击动作
            const float MontageLength =
                AnimInstance->Montage_Play(HitReactMontage);

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("[EnemyDummy] Hit React Montage Length: %.3f"),
                MontageLength
            );
        }
    }

    return ActualDamage;
}

bool AEnemyDummy::BeginMeleeAttack()
{
    if (bIsDead)
    {
        return false;
    }

    

    if (!IsValid(MeleeCombatComponent))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("MeleeCombatComponent is invalid")
        );
        return false;
    }

    if (!IsValid(AttackMontage))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("AttackMontage is invalid")
        );
        return false;
    }

    // ACharacter 自带 Mesh，不再通过 FindComponentByClass 查找
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (!IsValid(AnimInstance))
    {
        return false;
    }

    MeleeCombatComponent->BeginAttack();

    const float PlayLength =
        AnimInstance->Montage_Play(AttackMontage);
    
    UE_LOG(LogTemp, Warning, TEXT("[Enemy] Attack Started"));
    
    if (PlayLength <= 0.0f)
    {
        MeleeCombatComponent->EndAttack();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[Enemy] Failed to play AttackMontage")
        );

        return false;
    }

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(
        this,
        &AEnemyDummy::OnAttackMontageEnded
    );

    AnimInstance->Montage_SetEndDelegate(
        EndDelegate,
        AttackMontage
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[Enemy] Attack Montage started. Length: %.3f"),
        PlayLength
    );

    return true;
}

void AEnemyDummy::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    if (IsValid(MeleeCombatComponent))
    {
        MeleeCombatComponent->EndAttack();
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("%s died."),
        *GetName()
    );

    SetActorEnableCollision(false);

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (IsValid(AnimInstance))
    {
        if (IsValid(AttackMontage))
        {
            AnimInstance->Montage_Stop(0.1f, AttackMontage);
        }

        if (IsValid(DeathMontage))
        {
            const float MontageLength =
                AnimInstance->Montage_Play(DeathMontage);

            if (MontageLength > 0.0f)
            {
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(
                    this,
                    &AEnemyDummy::OnDeathMontageEnded
                );

                AnimInstance->Montage_SetEndDelegate(
                    EndDelegate,
                    DeathMontage
                );

                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("[EnemyDummy] Death Montage started. Length: %.3f"),
                    MontageLength
                );

                return;
            }
        }
    }

    // 没有可播放的死亡动画时，短暂延迟后销毁
    SetLifeSpan(0.1f);
}

void AEnemyDummy::OnAttackMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
)
{
    if (Montage != AttackMontage)
    {
        return;
    }

    if (IsValid(MeleeCombatComponent))
    {
        MeleeCombatComponent->EndAttack();
    }

    OnAttackFinished.Broadcast(this, bInterrupted);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[Enemy] Attack Montage ended. Interrupted: %s"),
        bInterrupted ? TEXT("true") : TEXT("false")
    );
}

void AEnemyDummy::OnDeathMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
)
{
    if (Montage != DeathMontage)
    {
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Death Montage ended. Interrupted: %s"),
        bInterrupted ? TEXT("true") : TEXT("false")
    );

    Destroy();
}

void AEnemyDummy::UpdateHealthBar()
{
    if (!IsValid(HealthBarWidget))
    {
        return;
    }

    UEnemyHealthBarWidget* HealthBar =
        Cast<UEnemyHealthBarWidget>(
            HealthBarWidget->GetUserWidgetObject()
        );

    if (!IsValid(HealthBar))
    {
        return;
    }

    const float HealthPercent =
        MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;

    HealthBar->SetHealthPersent(HealthPercent);
}