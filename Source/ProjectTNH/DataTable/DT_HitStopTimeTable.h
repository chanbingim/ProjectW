// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_HitStopTimeTable.generated.h"

USTRUCT(BlueprintType)
struct FHitStopTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float StopTime = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_HitStopTimeTable : public UDataTable
{
	GENERATED_BODY()
	
};
