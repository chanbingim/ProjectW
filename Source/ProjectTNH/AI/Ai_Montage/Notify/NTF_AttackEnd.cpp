// Fill out your copyright notice in the Description page of Project Settings.


#include "NTF_AttackEnd.h"
#include "../../Ai_CharacterBase/Ai_CharacterBase.h"

void UNTF_AttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto BaseCharacter = Cast<AAi_CharacterBase>(MeshComp->GetOwner());

	if (nullptr != BaseCharacter)
	{
		BaseCharacter->IAttackEnd();
	}
}