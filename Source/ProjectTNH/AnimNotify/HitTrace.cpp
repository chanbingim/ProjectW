// Fill out your copyright notice in the Description page of Project Settings.


#include "HitTrace.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterController.h"

void UHitTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter)
	{
		BaseCharacter->GetDidItHitComponent()->ToggleTraceCheck(true);
	}
}

void UHitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UHitTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter)
	{
		BaseCharacter->GetDidItHitComponent()->ToggleTraceCheck(false);
	}
}
