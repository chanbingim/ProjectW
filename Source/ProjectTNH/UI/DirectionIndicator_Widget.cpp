// Fill out your copyright notice in the Description page of Project Settings.


#include "DirectionIndicator_Widget.h"
#include "../Character/BaseCharacterClass.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/TNHCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectTNH/ProjectTNH.h"

void UDirectionIndicator_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	//SetDesiredSizeInViewport(WidgetSize);
}

void UDirectionIndicator_Widget::NativeTick(const FGeometry& MyGeometry, float DeletaTime)
{
	Super::NativeTick(MyGeometry, DeletaTime);

	auto parentActor = Cast<ABaseCharacterClass>(OwnerActor);
	auto Player = Cast<ATNHCharacter>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());

	if (nullptr != parentActor && Player->GetLockOn())
	{
		FVector2D ScreenPos;

		if (parentActor->CurrentState >= ECharacterState::Stagger)
		{
			SetDirWidgetColor(parentActor->pawnType, EType::None, parentActor->CurrentDirection);
		}	
		else if (parentActor->CurrentState <= ECharacterState::Move 
			|| parentActor->CurrentState == ECharacterState::Guard 
			|| parentActor->CurrentState == ECharacterState::GuardHit)
		{
			SetDirWidgetColor(parentActor->pawnType, EType::Guard, parentActor->CurrentDirection);
		}
		else if (parentActor->CurrentState == ECharacterState::Attack)
		{
			SetDirWidgetColor(parentActor->pawnType, EType::Attack, parentActor->CurrentDirection);
		}
		else if (parentActor->CurrentState == ECharacterState::Dodge)
		{
			SetDirWidgetColor(parentActor->pawnType, EType::None, ECombatDirection::None);
		}
	}
}
