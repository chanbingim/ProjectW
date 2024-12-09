// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_ExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FExecutionData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Execution_Attacker = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Execution_Victim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BetweenDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> NextComboExecutions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExecutionInRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExecutionOutRange = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_ExecutionData : public UDataTable
{
	GENERATED_BODY()
	
};
