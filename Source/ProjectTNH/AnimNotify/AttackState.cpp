// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackState.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Character/StateMachine.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/ProjectTNH.h"
#include "ProjectTNH/UI/HUD/TNHMainHUD.h"
#include "ProjectTNH/UI/CustomWidget_PlayerStateBarBase.h"
#include "ProjectTNH/PlayerController/TNHPlayerController.h"
#include "Components/WidgetComponent.h"
#include "ProjectTNH/UI/DirectionIndicator_Widget.h"

void UAttackState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter && BaseCharacter->Dr_widgetComp)
	{
		UDirectionIndicator_Widget* DirectionIndicator = Cast<UDirectionIndicator_Widget>(BaseCharacter->Dr_widgetComp->GetWidget());

		if (DirectionIndicator)
		{
			DirectionIndicator->OnAttack(BaseCharacter->AttackDirection, BaseCharacter->pawnType, TotalDuration);
		}

		if (BaseCharacter->TargetActor)
		{
			ATNHCharacter* Player = Cast<ATNHCharacter>(BaseCharacter->TargetActor);

			if (Player && Player->GetIsLockOnIngaged())
			{
				if (Player->TargetActor != BaseCharacter)
				{
					Player->OnLockOnState(true, BaseCharacter, false, false);
				}
			}
		}
	}
}

void UAttackState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter && BaseCharacter->Dr_widgetComp)
	{
		UDirectionIndicator_Widget* DirectionIndicator = Cast<UDirectionIndicator_Widget>(BaseCharacter->Dr_widgetComp->GetWidget());

		if (DirectionIndicator)
		{
			DirectionIndicator->EndAttack();
		}

		if (BaseCharacter->TargetActor)
		{
			ATNHCharacter* Player = Cast<ATNHCharacter>(BaseCharacter->TargetActor);

			if (Player && Player->GetIsLockOnIngaged())
			{
				if (Player->TargetActor != BaseCharacter)
				{
					Player->OnLockOnState(false, BaseCharacter, false, false);
				}
			}
		}
	}
}
