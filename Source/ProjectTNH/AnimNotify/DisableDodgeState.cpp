// Fill out your copyright notice in the Description page of Project Settings.


#include "DisableDodgeState.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Character/StateMachine.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/ProjectTNH.h"

void UDisableDodgeState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (PlayerCharacter)
	{
		PlayerCharacter->DisableDodge = true;
	}
}

void UDisableDodgeState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UDisableDodgeState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (PlayerCharacter)
	{
		PlayerCharacter->DisableDodge = false;
	}
}
