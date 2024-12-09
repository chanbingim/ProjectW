// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_AIStatus.generated.h"

USTRUCT(BlueprintType)
struct FAIStatusRow :public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightAngle = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightRadius = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float R_AttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StrafeDistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StrafeDistanceTol = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FirstAttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ComboAttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float NormalAttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CrashAttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CancleAttackPBT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackCoolTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CrashAttackCoolTime = 0;
};
