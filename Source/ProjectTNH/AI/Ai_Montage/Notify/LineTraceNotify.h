// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "LineTraceNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ULineTraceNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

public:
	virtual bool Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;
};
 