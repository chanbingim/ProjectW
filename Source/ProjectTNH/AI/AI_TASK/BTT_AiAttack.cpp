// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AiAttack.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "ProjectTNH/AttackListComponents.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectTNH/Character/CharacterState.h"

UBTT_AiAttack::UBTT_AiAttack()
{
	bNotifyTick = true;
	bIsAttack = false;
	NodeName = "AiAttack";
}

void UBTT_AiAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsAttack)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if(!bIsAttack && AICharacter->GetAttackListComponents()->GetSelectComboList().Num() <= 0)
	{
		auto _Target = Cast<ABaseCharacterClass>(AICharacter->TargetActor);
		if (nullptr == _Target)
			return;

		if (_Target->pawnType == EPawnType::Player)
			AICharacter->IAttackEnd();
	}
		
}

EBTNodeResult::Type UBTT_AiAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AICharacter = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	OwnerController = Cast<AAi_CharacterController>(AICharacter->GetController());
	auto _Target = Cast<ABaseCharacterClass>(AICharacter->TargetActor);

	if (nullptr == AICharacter || nullptr == OwnerController)
		return EBTNodeResult::Failed;
	
	if (AICharacter->CurrentState <= ECharacterState::Guard &&
		!AICharacter->bIsAttackCoolTime)
	{
		OwnerController->StopMovement();

		AICharacter->CurrentState = ECharacterState::Attack;
		bIsAttack = true;
		AICharacter->bIsAttackCoolTime = true;

		if (_Target->pawnType >= EPawnType::EliteSword)
		{
			if (!AttackTargetToPlayer())
			{
				AICharacter->IAttackEnd();
				return EBTNodeResult::Failed;;
			}
		}
		else
		{
			AICharacter->AttackSelect(-2, _Target);
		}

		AICharacter->FOnAttackEnd.AddLambda([this]()->void
			{
				bIsAttack = false;
			});

		return EBTNodeResult::InProgress;
	}

	AICharacter->IAttackEnd();
	return EBTNodeResult::Failed;
}

bool UBTT_AiAttack::AttackTargetToPlayer()
{
	float randomFloat = FMath::RandRange(0.0f, 1.0f);

	float PBT;
	float blancePercen = AICharacter->balance / AICharacter->Maxbalance;

	if (AICharacter->LowBalancePercent < blancePercen)
		PBT = OwnerController->PropertiesData->FirstAttackPBT;
	else
		PBT = OwnerController->PropertiesData->FirstAttackPBT - AICharacter->LowBalanceWeight;

	if (randomFloat <= PBT)
	{
		float ComboPBT = OwnerController->PropertiesData->ComboAttackPBT;
		float RandomFloat = FMath::RandRange(0.0f, ComboPBT);
		if (RandomFloat <= ComboPBT)
			AICharacter->MaxComboIndex = FMath::RandRange(2, MaxAttackComboCount);
		else
			AICharacter->MaxComboIndex = 1;

		if (!AICharacter->GetAttackListComponents()->MakeAttackComboList(AICharacter->MaxComboIndex))
			return false;
		
		AICharacter->CurrentComboIndex = 0;
		AICharacter->AttackSelect(0, AICharacter->TargetActor);
	}
	else
	{
		AICharacter->CurrentComboIndex = 0;
		return false;
	}
	return true;
}