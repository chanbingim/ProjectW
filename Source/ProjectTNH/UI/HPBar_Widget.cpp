// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBar_Widget.h"
#include "Components/ProgressBar.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void UHPBar_Widget::BindCustomDelegate()
{
	if (OwnerCharacter)
	{
		OwnerCharacter->OnHpChanged.AddUObject(this, &UHPBar_Widget::UpdateHP);
		OwnerCharacter->OnStaminaChanged.AddUObject(this, &UHPBar_Widget::UpdateStamina);
	}
}

void UHPBar_Widget::UpdateHP()
{
	if (OwnerCharacter)
	{
		HP->SetPercent(OwnerCharacter->GetHealthRatio());
	}
}

void UHPBar_Widget::UpdateStamina()
{
	//if (OwnerCharacter)
	//{
	//	Stamina->SetPercent(OwnerCharacter->GetStaminaRatio());
	//}
}
