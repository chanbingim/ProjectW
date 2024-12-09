// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckCombo.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterController.h"

void UCheckCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (OwnerPawn == nullptr) return;

	if (OwnerPawn->pawnType == EPawnType::Player)
	{
		Cast<ATNHCharacter>(OwnerPawn)->CheckNextAttack();
	}
}

void UCheckCombo::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UCheckCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (OwnerPawn == nullptr) return;

	if (OwnerPawn->pawnType == EPawnType::Player)
	{
		Cast<ATNHCharacter>(OwnerPawn)->ContinueAttack();
	}
	else
	{
		auto AIPawn = Cast<AAi_CharacterBase>(OwnerPawn);
		
		if (AIPawn->pawnType < EPawnType::EliteSword)
			return;

		AAi_CharacterController* Controller = Cast<AAi_CharacterController>(OwnerPawn->GetController());
		if (AIPawn->CurrentComboIndex >= AIPawn->MaxComboIndex ||
			AIPawn->CurrentState != ECharacterState::Attack || Controller == nullptr)
		{
			AIPawn->CurrentComboIndex = 0;
			AIPawn->IAttackEnd();
			return;
		}

		int Index = Controller->AttackSelectFUNC();
		
		if(AIPawn->TargetActor != nullptr)
			AIPawn->AttackSelect(Index, AIPawn->TargetActor);
	}
	return;
}
