// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckEQS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBTD_CheckEQS : public UBTDecorator
{
	GENERATED_BODY()
public :
	UBTD_CheckEQS();

protected :
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
