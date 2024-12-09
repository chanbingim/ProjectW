// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_TurnAiToEnemy.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../../Character/TNHCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_TurnAiToEnemy::UBTT_TurnAiToEnemy()
{
	NodeName = "TurnAi";
}

EBTNodeResult::Type UBTT_TurnAiToEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto EnemyCharacter = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	auto EnemyController = Cast<AAi_CharacterController>(EnemyCharacter->GetController());
	if (nullptr == EnemyCharacter)
		return EBTNodeResult::Failed;
	
	EnemyController->SetFocus(EnemyCharacter->TargetActor);
	return EBTNodeResult::Succeeded;
}







