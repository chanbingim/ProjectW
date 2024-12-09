// Fill out your copyright notice in the Description page of Project Settings.


#include "SetHandIK.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void USetHandIK::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ATNHCharacter* Character = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->bUseHandIk = BeginValue;
	}
}

void USetHandIK::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void USetHandIK::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ATNHCharacter* Character = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->bUseHandIk = EndValue;
	}
}
