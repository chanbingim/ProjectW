// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ToGuard.h"
#include "../../AI/Ai_CharacterBase/Ai_CharacterController.h"
#include "../../AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ToGuard::UBTT_ToGuard()
{
	NodeName = "ToGuard";
}

EBTNodeResult::Type UBTT_ToGuard::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAi_CharacterController* OwnningController = Cast<AAi_CharacterController>(OwnerComp.GetAIOwner()->GetPawn()->GetController());
	AAi_CharacterBase* OwnningPawn = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());

	float randFloat = FMath::RandRange(0.0f, 1.0f);
	if (OwnningPawn->CurrentState == ECharacterState::Idle)
	{
		if (randFloat <= 1.0f)
		{
			OwnningController->ChangeParringState(true);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Succeeded;
}