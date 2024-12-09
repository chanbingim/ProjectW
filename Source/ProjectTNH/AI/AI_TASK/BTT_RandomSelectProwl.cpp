// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RandomSelectProwl.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RandomSelectProwl::UBTT_RandomSelectProwl()
{
	bNotifyTick = false;
	NodeName = "RandomSelectProwl";
}

EBTNodeResult::Type UBTT_RandomSelectProwl::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AICharacter = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	auto OwnerController = Cast<AAi_CharacterController>(AICharacter->GetController());
	auto _target = Cast<ABaseCharacterClass>(AICharacter->TargetActor);

	if (nullptr == AICharacter || nullptr == OwnerController)
		return EBTNodeResult::Failed;

	if (AICharacter->CurrentState >= ECharacterState::GuardHit ||
		!AICharacter->bIsAttackCoolTime)
	{
		return EBTNodeResult::Succeeded;
	}

	float BaseDistance = OwnerController->PropertiesData->AttackRange;
	float MaxDistance = OwnerController->PropertiesData->StrafeDistanceTol;

	float StrafeRange = FMath::RandRange(BaseDistance - MaxDistance, BaseDistance + MaxDistance);

	int SelectRandom = FMath::RandRange(1, 5);
	float RandomDegree = FMath::DegreesToRadians(360.f * FMath::Sqrt(static_cast<float>(SelectRandom)));

	if (AICharacter->TargetActor == nullptr)
		return EBTNodeResult::Failed;

	MoveLoc.X = AICharacter->TargetActor->GetActorLocation().X + StrafeRange * FMath::Cos(RandomDegree);
	MoveLoc.Y = AICharacter->TargetActor->GetActorLocation().Y + StrafeRange * FMath::Sin(RandomDegree);
	MoveLoc.Z = AICharacter->GetActorLocation().Z;

	OwnerController->SetFocus(AICharacter->TargetActor);
	OwnerController->MoveToLocation(MoveLoc);
	return EBTNodeResult::Succeeded;
}



