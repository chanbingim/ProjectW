// Fill out your copyright notice in the Description page of Project Settings.


#include "OverrideState.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void UOverrideState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());
	if (nullptr == PlayerCharacter || PlayerCharacter->CurrentState == ECharacterState::Dead) return;

	if (PlayerCharacter)
	{
		PlayerCharacter->FSM_PlayerChangeState(OverrideState);
	}
}
