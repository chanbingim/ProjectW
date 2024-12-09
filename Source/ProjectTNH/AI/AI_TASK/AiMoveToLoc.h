// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AiMoveToLoc.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UAiMoveToLoc : public UBTTaskNode
{
	GENERATED_BODY()
private :
	FVector TargetPoint;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public :
	UAiMoveToLoc();
};
