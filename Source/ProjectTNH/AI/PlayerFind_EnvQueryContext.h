// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "PlayerFind_EnvQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UPlayerFind_EnvQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
protected :
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const;
};
