// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MissionDataTable.generated.h"

USTRUCT(BlueprintType)
struct FMissionData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MissionName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MissionExplication = "";
};




/**
 * 
 */
UCLASS()
class PROJECTTNH_API UMissionDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
