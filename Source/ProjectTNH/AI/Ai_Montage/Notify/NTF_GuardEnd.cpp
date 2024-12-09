// Fill out your copyright notice in the Description page of Project Settings.


#include "NTF_GuardEnd.h"
#include "../AiMontage_Interface.h"

void UNTF_GuardEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	IAiMontage_Interface* AIPawNInterface = Cast<IAiMontage_Interface>(MeshComp->GetOwner());

	if (nullptr != AIPawNInterface)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AttackCheck"));
	}
}
