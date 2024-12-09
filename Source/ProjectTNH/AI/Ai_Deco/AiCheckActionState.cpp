// Fill out your copyright notice in the Description page of Project Settings.


#include "AiCheckActionState.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/Character/CharacterState.h"

UAiCheckActionState::UAiCheckActionState()
{
	NodeName = "CheckActionable";
}

bool UAiCheckActionState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool flag = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto CurrentController = Cast<AAi_CharacterController>(OwnerComp.GetAIOwner()->GetPawn()->GetController());
	if (nullptr == CurrentController)
		return false;

	auto CurrentPawn = Cast<ABaseCharacterClass>(CurrentController->GetPawn());
	if (CurrentPawn->CurrentState == ECharacterState::Missed ||
		CurrentPawn->CurrentState == ECharacterState::Dead ||
		CurrentPawn->CurrentState == ECharacterState::Stagger ||
		CurrentPawn->CurrentState == ECharacterState::Stun ||
		CurrentPawn->CurrentState == ECharacterState::Execution)
		return false;
	else
		return true;
}


