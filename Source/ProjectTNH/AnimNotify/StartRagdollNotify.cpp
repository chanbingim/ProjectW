// Fill out your copyright notice in the Description page of Project Settings.

#include "StartRagdollNotify.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectTNH/Character/TNHCharacter.h"



void UStartRagdollNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (OwnerPawn)
	{
		OwnerPawn->SetFixedRagdoll();
	}
}