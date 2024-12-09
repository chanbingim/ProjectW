// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckEQS.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_CheckEQS::UBTD_CheckEQS()
{
	NodeName = "EQSAbleCheck";
}

bool UBTD_CheckEQS::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	auto OwnerAiController = Cast<AAi_CharacterController>(OwnerComp.GetAIOwner());
	auto ControllingPawn = Cast<ABaseCharacterClass>(OwnerAiController->GetPawn());
	auto _Target = Cast<ABaseCharacterClass>(ControllingPawn->TargetActor);

	if (_Target == nullptr || ControllingPawn->CurrentState < ECharacterState::Attack)
		return bResult;

	if (_Target->pawnType > EPawnType::NormalArcher &&
		!OwnerAiController->GetBlackboardComponent()->GetValueAsBool(AAi_CharacterController::Key_AttackCoolTime))
		return true;
	
	return bResult;
}
