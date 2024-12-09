// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAttackDirectionChange.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/DataTable/DT_AttackRow.h"

void UBossAttackDirectionChange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation);

	auto OwenrCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (OwenrCharacter != nullptr)
	{
		OwenrCharacter->AttackDirection = ChangeDirection;
		OwenrCharacter->CurrentDirection = ChangeDirection;
		OwenrCharacter->AttackReaction = ChangeReactionType;
	}
}
