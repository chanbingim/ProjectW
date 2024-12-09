// Fill out your copyright notice in the Description page of Project Settings.


#include "AiCharacter_AnimBase.h"
#include "Animation/AnimMontage.h"
#include "../../Character/BaseCharacterClass.h"
#include "../Ai_CharacterBase/Ai_CharacterController.h"
#include "Ai_CharacterBase.h"
#include "KismetAnimationLibrary.h"

void UAiCharacter_AnimBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateRotationTime += DeltaSeconds;

	OwnningAI = Cast<AAi_CharacterBase>(GetOwningActor());
	if (OwnningAI != nullptr)
	{
		CheckGuardDir();
		CheckLookAtDir();
	}
}

void UAiCharacter_AnimBase::NativeBeginPlay()
{
	LeftGuard = false;
}

void UAiCharacter_AnimBase::BlendOutAnimation(UAnimMontage* animMontage, bool bInterrupted)
{


}

void UAiCharacter_AnimBase::CheckGuardDir()
{
	auto GuardDirection = OwnningAI->GetCurrentGuardDriection();

	switch (GuardDirection)
	{
	case ECombatDirection::Right:
		LeftGuard = false;
		break;

	case ECombatDirection::Left:
		LeftGuard = true;
		break;
	}
}

void UAiCharacter_AnimBase::CheckLookAtDir()
{
	if (OwnningAI->TargetActor == nullptr)
		return;
	else
	{
		FVector Dir = OwnningAI->GetActorLocation()- OwnningAI->TargetActor->GetActorLocation();
		LookDir = Dir.Rotation();
	}
}