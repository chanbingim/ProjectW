// Fill out your copyright notice in the Description page of Project Settings.


#include "SetExecutionable.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void USetExecutionable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (OwnerPawn)
	{
		OwnerPawn->ExecutionAble = SetValue;
	}
}
