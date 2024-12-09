// Fill out your copyright notice in the Description page of Project Settings.


#include "AiMoveToLoc.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../Ai_CharacterBase/AiCharacter_AnimBase.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UAiMoveToLoc::UAiMoveToLoc()
{
	bNotifyTick = true;
	NodeName = "AiMoveToLoc";
}

EBTNodeResult::Type UAiMoveToLoc::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AICharacter = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	auto OwnerController = Cast<AAi_CharacterController>(AICharacter->GetController());
	if (nullptr == AICharacter || nullptr == OwnerController)
		return EBTNodeResult::Failed;

	if (AICharacter->TargetActor != nullptr || AICharacter->CurrentState <= ECharacterState::Run)
	{
		float dis = AICharacter->GetDistanceTo(AICharacter->TargetActor);
		if (dis <= OwnerController->PropertiesData->AttackRange)
			return EBTNodeResult::Succeeded;

		OwnerController->SetFocus(AICharacter->TargetActor);
		OwnerController->MoveToActor(AICharacter->TargetActor);

		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Succeeded;
}


