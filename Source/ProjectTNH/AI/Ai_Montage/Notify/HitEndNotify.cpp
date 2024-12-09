// Fill out your copyright notice in the Description page of Project Settings.


#include "HitEndNotify.h"
#include "../AiMontage_Interface.h"

void UHitEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	IAiMontage_Interface* AIPawnInterface = Cast<IAiMontage_Interface>(MeshComp->GetOwner());

	if (nullptr != AIPawnInterface)
	{
		AIPawnInterface->IHitEnd();
	}
}
