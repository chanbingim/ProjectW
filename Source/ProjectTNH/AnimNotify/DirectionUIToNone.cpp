// Fill out your copyright notice in the Description page of Project Settings.


#include "DirectionUIToNone.h"
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

void UDirectionUIToNone::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (BaseCharacter && BaseCharacter->Dr_widgetComp)
	{
		UDirectionIndicator_Widget* DirectionIndicator = Cast<UDirectionIndicator_Widget>(BaseCharacter->Dr_widgetComp->GetWidget());

		if (DirectionIndicator)
		{
			//DirectionIndicator->OnAttack(BaseCharacter->AttackDirection, BaseCharacter->pawnType, 0.f);
		}
	}
}
