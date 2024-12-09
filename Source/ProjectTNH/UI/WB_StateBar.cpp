// Fill out your copyright notice in the Description page of Project Settings.


#include "WB_StateBar.h"
#include "../Character/BaseCharacterClass.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWB_StateBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Dele_HPSetting.BindDynamic(this, &UWB_StateBar::SetHpState);
	Dele_BalanceSetting.BindDynamic(this, &UWB_StateBar::SetBalanceState);
	Dele_NameSetting.BindDynamic(this, &UWB_StateBar::SetTextBlock);
}

void UWB_StateBar::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWB_StateBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
}

void UWB_StateBar::SetHpState(float _CurrentHP, float _MaxHP)
{
	MaxHP = _MaxHP;
	CurrentHP = _CurrentHP;

	StateOption.HPBar->SetPercent(CurrentHP / MaxHP);


	Super::ForceLayoutPrepass();
}

void UWB_StateBar::SetBalanceState(float CurrentBal, float MaxBal)
{
	StateOption.BalanceBar->SetPercent(CurrentBal / MaxBal);
	Super::ForceLayoutPrepass();
}

void UWB_StateBar::Play_AnimationBar(float _CurrentData, float _MaxData)
{
	
}

void UWB_StateBar::SetTextBlock(FString name)
{
	for (auto Data : NameOption)
	{
		if (Data.OwnningBlockPawnType >= EPawnType::EliteSword)
		{
			if (Data.NameBlock != nullptr)
			{
				if(name != "")
					Data.NameBlock->SetText(FText::FromString(name));
				else
					Data.NameBlock->SetText(FText::FromString(
					UEnum::GetDisplayValueAsText(Data.OwnningBlockPawnType).ToString()));
			}
		}
	}
	Super::ForceLayoutPrepass();
}
