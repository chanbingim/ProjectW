// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcedMoveToTarget.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterController.h"

void UForcedMoveToTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	
	if (BaseCharacter)
	{
		if (ShouldMove)
		{
			BaseCharacter->bShouldMoveToTarget = true;
			BaseCharacter->MoveSpeed = MoveSpeed;
		}

		if (ShouldTurn)
		{
			BaseCharacter->bShouldTurnToTarget = true;
			BaseCharacter->ChangeControllerRotation(true);
		}
	}
}

void UForcedMoveToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	
	if (BaseCharacter != nullptr)
	{
		BaseCharacter->TrackingTarget(FrameDeltaTime, MoveSpeed);
		BaseCharacter->TurnToTarget(FrameDeltaTime);

		BaseCharacter->UpdateAttackLocation();
	}
}

void UForcedMoveToTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter)
	{
		if (ShouldMove)
		{
			BaseCharacter->bShouldMoveToTarget = false;
		}

		if (ShouldTurn)
		{
			BaseCharacter->bShouldTurnToTarget = false;
		}

		BaseCharacter->StopAttackTracking();
	}
}
