// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveDataStruct.generated.h"

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_USTRUCT_BODY()

public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Player_Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Player_Balance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	int Mission_Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FString CurrentLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FVector Player_Location;

};
