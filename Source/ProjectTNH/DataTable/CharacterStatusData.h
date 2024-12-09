// Fl out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterStatusData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStatusTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxBalance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DefaultThreat = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RT_Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RT_Balance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RV_Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RV_Balance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RV_Threat = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WalkSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RunSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CharacterRadius = 0;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UCharacterStatusData : public UDataTable
{
	GENERATED_BODY()
	
};
