// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_TurnAiToEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBTT_TurnAiToEnemy : public UBTTaskNode
{
	GENERATED_BODY()

private :
	UPROPERTY(EditAnywhere)
	bool AbleMotionWrap;
public :
	UBTT_TurnAiToEnemy();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	float DelayTime = 0.0f;
};
