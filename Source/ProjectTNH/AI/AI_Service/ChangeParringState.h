// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ChangeParringState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UChangeParringState : public UBTService
{
	GENERATED_BODY()

protected :
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
	bool bIsCanCnageGuardDirection(AAi_CharacterBase* _Character);
public:
	UChangeParringState();
};
