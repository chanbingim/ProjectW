// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AiAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBTT_AiAttack : public UBTTaskNode
{
	GENERATED_BODY()
private :
	class AAi_CharacterBase* AICharacter;
	class AAi_CharacterController* OwnerController;

protected :
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

public :
	UBTT_AiAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	bool AttackTargetToPlayer();

	UPROPERTY()
	bool bIsAttack;

	UPROPERTY(EditAnywhere)
	int MaxAttackComboCount;
};
