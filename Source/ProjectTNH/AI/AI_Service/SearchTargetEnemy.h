// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SearchTargetEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API USearchTargetEnemy : public UBTService
{
	GENERATED_BODY()

private :
	class AAi_CharacterController* CurrentController;
	class ABaseCharacterClass* OwnerPawn;
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public :
	USearchTargetEnemy();

	bool FailSearchTarget();
};
