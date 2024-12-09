// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckAttackRange.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "Character/ALSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_CheckAttackRange::UBTD_CheckAttackRange()
{
	NodeName = "CheckAttackRange";
}

bool UBTD_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto OwnerAiController = Cast<AAi_CharacterController>(OwnerComp.GetAIOwner());
	auto ControllingPawn = Cast<ABaseCharacterClass>(OwnerAiController->GetPawn());
	auto Target = ControllingPawn->TargetActor;
	float Range = OwnerAiController->PropertiesData->AttackRange;

	if (nullptr == Target)
		return false;

	bResult = (ControllingPawn->GetDistanceTo(Cast<AActor>(Target)) <= Range);
	return bResult;
}