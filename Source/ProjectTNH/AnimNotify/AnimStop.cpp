// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimStop.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void UAnimStop::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (OwnerPawn)
	{
		OwnerPawn->GetMesh()->GetAnimInstance()->Montage_Pause();
	}
}
