// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"


class AEnemyDummy;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyAttackFinished, AEnemyDummy*, bool);

enum class EEnemyAIState :uint8
{
	Idle,
	Chasing,
	ReadyToAttack,
	Attacking
};


UCLASS()
class MELEEPROTOTYPE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	AEnemyAIController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
	// 防止移动请求失败时每帧重试
	float MoveRetryTimer = 0.0f;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float DetectionRadius = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
	float StopDistance = 170.f;

private:
	EEnemyAIState CurrentState = EEnemyAIState::Idle;

	void StartChasing(ACharacter* PlayerCharacter);

	// 接收敌人攻击结束通知
	void HandleEnemyAttackFinished(AEnemyDummy* Enemy, bool bInterrupted);

	// 记录绑定的敌人，避免解除控制后仍保留订阅关系
	TWeakObjectPtr<AEnemyDummy> BoundEnemy;

	// 保存绑定句柄，之后用于解绑
	FDelegateHandle AttackFinishedDelegateHandle;

	bool bNeedsAIReevaluation = false;
};
