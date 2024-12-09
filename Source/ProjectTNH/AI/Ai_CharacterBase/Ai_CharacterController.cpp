// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai_CharacterController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AiCharacter_AnimBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "ProjectTNH/DataTable/DT_AnimationTable.h"
#include "ProjectTNH/BoundBoxBase.h"
#include "../TargetSelectComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectTNH/AttackListComponents.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "../UAiCommander.h"
#include "../AiMoveComponents.h"
#include "Ai_CharacterBase.h"


const FName AAi_CharacterController::Key_TargetEnemy(TEXT("TargetEnemy"));
const FName AAi_CharacterController::Key_LevelObject(TEXT("LevelObject"));
const FName AAi_CharacterController::Key_AttackCoolTime(TEXT("bIsAttackCooltime"));
const FName AAi_CharacterController::Key_GuardCoolTime(TEXT("bIsGuardCooltime"));
const FName AAi_CharacterController::Key_StrafeRange(TEXT("StrafeRange"));
const FName AAi_CharacterController::Key_CurrentState(TEXT("CurrentState"));
const FName AAi_CharacterController::Key_EQSMoveLoc(TEXT("EQSMoveLoc"));

AAi_CharacterController::AAi_CharacterController(FObjectInitializer const& object_initializer)
{
	BehaviorComp = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	Blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
	if (AiPerceptionCom != nullptr)
	{
		if (AiPerceptionCom->OnPerceptionUpdated.Contains(this, FName(TEXT("OnTargetDetected"))) == false)
		{
			AiPerceptionCom->OnPerceptionUpdated.AddDynamic(this, &AAi_CharacterController::OnTargetDetected);
		}
	}
}

void AAi_CharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	if (BTAsset != nullptr)
	{
		BehaviorComp->StartTree(*BTAsset);
	}
		
}

void AAi_CharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAi_CharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Blackboard)
	{
		if(BTAsset != nullptr && BTAsset->GetBlackboardAsset() != nullptr)
			Blackboard->InitializeBlackboard(*BTAsset->GetBlackboardAsset());

		if (nullptr != OwnningPawn)
		{
			if (OwnningPawn->MissionObject != nullptr)
			{
				OwnningPawn->CurrentState = ECharacterState::Run;
				OwnningPawn->GetMoveComponents()->MovementAccordingToStatus();
			}
		}
	}
}

void AAi_CharacterController::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	
}

void AAi_CharacterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
}

void AAi_CharacterController::OnTargetDetected(const TArray<AActor*>& UpdatedActors)
{
	auto TargetPawn = Cast<ABaseCharacterClass>(OwnningPawn->TargetActor);

	if (nullptr == TargetPawn || TargetPawn->pawnType != EPawnType::Player)
	{
		auto Player = Cast<ABaseCharacterClass>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (UpdatedActors.Contains(Player) && Player->TeamNum != OwnningPawn->TeamNum)
			OwnningPawn->TargetActor = Player;

		if(OwnningPawn->CurrentState == ECharacterState::Run)
		OwnningPawn->SetCharacterWalkSpeed(OwnningPawn->Walkspeed);
	}

	if (TargetPawn != nullptr && TargetPawn->CurrentState <= ECharacterState::Dead)
	{
		GetBlackboardComponent()->SetValueAsObject(Key_TargetEnemy, TargetPawn);
		SetFocus(TargetPawn);
	}
	else
		SetFocus(nullptr);
		
}

void AAi_CharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (OwnningPawn->CurrentState == ECharacterState::Dead)
		return;

	if (Result.IsSuccess())
	{
		// 이동이 성공적으로 완료되었을 때 실행할 코드
		switch (OwnningPawn->CurrentState)
		{
		case ECharacterState::Move:
			
			break;

		case ECharacterState::Run:
			OwnningPawn->SetCharacterWalkSpeed(OwnningPawn->Walkspeed);
			break;
		}

		OwnningPawn->CurrentState = ECharacterState::Idle;
	}
	bIsMove = false;
}

void AAi_CharacterController::TargetRelase(AActor* actor)
{

}

int AAi_CharacterController::AttackSelectFUNC()
{
	float randomFloat = FMath::RandRange(0.0, 1.0);

	if (randomFloat <= PropertiesData->CrashAttackPBT &&
		OwnningPawn->GetAttackListComponents()->BreakAttackList.Num() > 0)
	{
		return -1;
	}

	return 0;
}

int32 AAi_CharacterController::selectEvansionWay()
{
	//데이터 테이블 참조 방식으로 바뀔수도 있음

	float randomInt = FMath::RandRange(0, 1);

	return randomInt;
}

void AAi_CharacterController::DeadAiPawn(AActor* KillerPawn)
{
	Super::DeadAiPawn(KillerPawn);

	if(BehaviorComp != nullptr)
		BehaviorComp->StopTree();

	Destroy();
}

void AAi_CharacterController::ChangeParringState(bool isEnter)
{
	int RandomInt = FMath::RandRange(0, 1);
	if (OwnningPawn->CurrentState != ECharacterState::Guard)
		return;

	if (isEnter)
	{
		switch (RandomInt)
		{
		case 0:
			if (OwnningPawn->GetCurrentGuardDriection() == ECombatDirection::Left)
			{
				OwnningPawn->CurrentDirection = ECombatDirection::Right;
			}
				
			break;

		case 1 :
			if (OwnningPawn->GetCurrentGuardDriection() == ECombatDirection::Right)
			{
				OwnningPawn->CurrentDirection = ECombatDirection::Left;
			}
				
			break;
		}

		OwnningPawn->CurrentState = ECharacterState::Guard;
	}
}


