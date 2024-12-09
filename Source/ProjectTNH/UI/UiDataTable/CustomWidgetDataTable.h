// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CustomWidgetDataTable.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCustomWidgetClass : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<class UUserWidget> CustomWidgetBase;
};

UCLASS()
class PROJECTTNH_API UCustomWidgetDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
