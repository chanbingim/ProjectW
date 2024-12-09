// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossAttackDirectionChange.generated.h"

enum class ECombatDirection : uint8;
enum class EReactionType :uint8;
/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBossAttackDirectionChange : public UAnimNotify
{
	GENERATED_BODY()
	
private :
	UPROPERTY(EditAnywhere)
	ECombatDirection ChangeDirection;

	UPROPERTY(EditAnywhere)
	EReactionType ChangeReactionType;


protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
