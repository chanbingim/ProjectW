// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckComboExecution.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void UCheckComboExecution::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (OwnerPawn == nullptr) return;

	if (OwnerPawn->pawnType == EPawnType::Player)
	{
		Cast<ATNHCharacter>(OwnerPawn)->CheckNextAttack();
	}
}

void UCheckComboExecution::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UCheckComboExecution::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (OwnerPawn == nullptr) return;

	if (OwnerPawn->pawnType == EPawnType::Player)
	{
		Cast<ATNHCharacter>(OwnerPawn)->ContinueAttack();
	}
}
