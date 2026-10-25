// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::SetHealthPersent(float Percent)
{
	if (!IsValid(HealthBar))
		return;

	HealthBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));

}
