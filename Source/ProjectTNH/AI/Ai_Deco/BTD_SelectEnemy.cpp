// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_SelectEnemy.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../TargetSelectComponent.h"
#include "../../Character/BaseCharacterClass.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_SelectEnemy::UBTD_SelectEnemy()
{
	NodeName = "BTD_SelectEnemy";
}

bool UBTD_SelectEnemy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return false;
}