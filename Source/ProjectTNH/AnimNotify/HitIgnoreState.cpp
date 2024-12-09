// Fill out your copyright notice in the Description page of Project Settings.


#include "HitIgnoreState.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Character/StateMachine.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/ProjectTNH.h"

void UHitIgnoreState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter)
	{
		BaseCharacter->HitIgnore = true;
	}
}

void UHitIgnoreState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UHitIgnoreState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter)
	{
		BaseCharacter->HitIgnore = false;
	}
}
