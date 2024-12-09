// Fill out your copyright notice in the Description page of Project Settings.


#include "NTF_AttackCheck.h"
#include "../AiMontage_Interface.h"
#include "DidItHitActorComponent.h"
#include "../../Ai_CharacterBase/Ai_CharacterBase.h"

void UNTF_AttackCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	IAiMontage_Interface* AIPawNInterface = Cast<IAiMontage_Interface>(MeshComp->GetOwner());

	if (nullptr != AIPawNInterface)
	{
		AAi_CharacterBase* Owner = Cast<AAi_CharacterBase>(MeshComp->GetOwner());
		Owner->GetDidItHitComponent()->ToggleTraceCheck(true);
	}
}
