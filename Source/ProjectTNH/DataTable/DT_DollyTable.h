// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_DollyTable.generated.h"

USTRUCT(BlueprintType)
struct FDollyTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float DollyDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float DollySpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float POV_Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float POV_ChangeSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float DelayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float ReturnSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DollySet")
	float POV_ReturnSpeed = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_DollyTable : public UDataTable
{
	GENERATED_BODY()
	
};
