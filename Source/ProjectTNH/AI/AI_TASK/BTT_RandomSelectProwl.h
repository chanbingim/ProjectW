// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RandomSelectProwl.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBTT_RandomSelectProwl : public UBTTaskNode
{
	GENERATED_BODY()
private :
	UPROPERTY(VisibleAnywhere)
	FVector MoveLoc;

protected :
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTT_RandomSelectProwl();

};
