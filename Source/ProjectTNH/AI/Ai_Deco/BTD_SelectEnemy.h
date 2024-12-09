// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_SelectEnemy.generated.h"

class AActor;
/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBTD_SelectEnemy : public UBTDecorator
{
	GENERATED_BODY()
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public :
	UBTD_SelectEnemy();
};
