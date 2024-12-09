// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchTargetEnemy.h"
#include "../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "../TargetSelectComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../Character/BaseCharacterClass.h"
#include "../TargetSelectComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

USearchTargetEnemy::USearchTargetEnemy()
{
	NodeName = "UpdateTargetEnemy";
	Interval = 2.0f;
}

bool USearchTargetEnemy::FailSearchTarget()
{
	if (nullptr == CurrentController || CurrentController->TAry_TargetEnemy.Num() <= 0)
		return false;

	return true;
}

void USearchTargetEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory,DeltaSeconds);

	
}