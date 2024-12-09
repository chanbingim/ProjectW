// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NTF_GuardEnd.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UNTF_GuardEnd : public UAnimNotify
{
	GENERATED_BODY()

protected :
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
