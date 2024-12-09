// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_AiETCDataRow.generated.h"


USTRUCT(BlueprintType)
struct FDT_ETC : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float GuardDegree = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ParringTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ThreatCorrection = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float DuelThreat = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float SoftStaggerTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float HardStaggerTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ParringCooltime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float DodgeCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float BackRollCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float UnbattleRollCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float DodgeInvTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ExcutionDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ExcutionAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ExcutionRegenHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Etc")
	float ThreatConst = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_AiETCDataRow : public UDataTable
{
	GENERATED_BODY()
	
};
