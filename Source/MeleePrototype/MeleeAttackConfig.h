#pragma once

#include "CoreMinimal.h"
#include "MeleeAttackConfig.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float TraceDistance = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float TraceRadius = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    float Damage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
    FVector TraceStartOffset = FVector(30.0f, 0.0f, 50.0f);
};