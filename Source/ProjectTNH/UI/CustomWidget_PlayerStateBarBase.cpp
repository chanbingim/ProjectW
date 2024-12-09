// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomWidget_PlayerStateBarBase.h"
#include "../Character/BaseCharacterClass.h"
#include "Kismet/GameplayStatics.h"
#include "WB_StateBar.h"
#include "HUD/TNHMainHUD.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UCustomWidget_PlayerStateBarBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();


}

void UCustomWidget_PlayerStateBarBase::NativeConstruct()
{
	Super::NativeConstruct();

	LockOnWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UCustomWidget_PlayerStateBarBase::VisbleLockOn(AActor* LockOnActor)
{
	ABaseCharacterClass* LockOnPawn = Cast<ABaseCharacterClass>(LockOnActor);
	UWB_StateBar* LockOnBar = Cast<UWB_StateBar>(LockOnWidget);

	if (LockOnPawn->StateBar == nullptr)
	{
		//LockOnPawn->StateBar = LockOnWidget;
		///LockOnBar->Dele_HPSetting.Execute(LockOnPawn->Health, LockOnPawn->MaxHealth);
		//LockOnBar->Dele_BalanceSetting.Execute(LockOnPawn->balance, LockOnPawn->Maxbalance);
		//LockOnBar->Dele_NameSetting.Execute(LockOnPawn->GetPawnType());
		LockOnWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCustomWidget_PlayerStateBarBase::HiddenLockOn()
{
	LockOnWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UCustomWidget_PlayerStateBarBase::MissionUIVisible(ESlateVisibility visible)
{
	if (MissionDataUI)
		MissionDataUI->SetVisibility(visible);
}
