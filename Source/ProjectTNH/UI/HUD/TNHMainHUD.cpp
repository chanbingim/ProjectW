// Fill out your copyright notice in the Description page of Project Settings.


#include "TNHMainHUD.h"
#include "../UiDataTable/CustomWidgetDataTable.h"
#include "../WB_StateBar.h"
#include "../BaseViewPortWidget.h"
#include "../../Character/PawnType.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ATNHMainHUD::ATNHMainHUD()
{
	WidgetPool.SetWorld(GetWorld());
	WidgetPool.SetDefaultPlayerController(UGameplayStatics::GetPlayerController(this, 0));
	WidgetPool.IsInitialized();
}

void ATNHMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	//if(IsValid(this))
	//	Init();
}

void ATNHMainHUD::Tick(float DeletaTime)
{
	Super::Tick(DeletaTime);

	
}

void ATNHMainHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CustomWidgetRemove("PlayerStateBar");

	if (this != nullptr)
		Destroy();
}

UUserWidget* ATNHMainHUD::CustomWidgetAdd(FString Name)
{
	TSubclassOf<class UUserWidget> WidgetBase = GetCustomWidget(Name);
	
	if (nullptr == WidgetBase)
		return nullptr;

	UUserWidget* TempWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetBase);
	if (Cur_ActiveWidget.Contains(TempWidget))
		return nullptr;

	if (TempWidget != nullptr)
	{
		Cur_ActiveWidget.Add(TempWidget);
		TempWidget->AddToViewport();
		return TempWidget;
	}
	
	return nullptr;
}

void ATNHMainHUD::CustomWidgetRemove(FString Name)
{
	TSubclassOf<class UUserWidget> WidgetBase = GetCustomWidget(Name);

	for (auto var : Cur_ActiveWidget)
	{
		if (WidgetBase == var->GetClass())
		{
			var->RemoveFromParent();
			Cur_ActiveWidget.Remove(var);
			return;
		}
	}
}

void ATNHMainHUD::Init()
{
	CustomWidgetAdd("PlayerStateBar");
}

UUserWidget* ATNHMainHUD::AddCustomWidgetFromWidgetPool(FString Name)
{
	TSubclassOf<class UUserWidget> WidgetBase = GetCustomWidget(Name);

	if (nullptr == WidgetBase)
		return nullptr;

	UUserWidget* tempwidget = WidgetPool.GetOrCreateInstance<UUserWidget>(WidgetBase);
	if (tempwidget != nullptr)
	{
		if (Name == "CombatDirection")
			tempwidget->SetDesiredSizeInViewport(FVector2D(0, 0));
		tempwidget->AddToViewport();
	}

	return tempwidget;
}

void ATNHMainHUD::RemoveCustomWidgetFromWidgetPool(UUserWidget* removeWidget)
{
	removeWidget->RemoveFromParent();
	WidgetPool.Release(removeWidget);
}

UUserWidget* ATNHMainHUD::FindCustomWidgetFromWidgetPool(AActor* _OwnningActor)
{
	if (_OwnningActor == nullptr)
		return nullptr;

	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	for (auto widget : ActiveWidgets)
	{
		auto BaseWidget = Cast<UBaseViewPortWidget>(widget);
		if (BaseWidget->GetWidgetParent() == _OwnningActor)
		{
			return widget;
		}
	}

	return nullptr;
}

UUserWidget* ATNHMainHUD::FindCustomWidgetFromWidgetPool(FString Name)
{
	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	TSubclassOf<class UUserWidget>WidgetBase = GetCustomWidget(Name);
	for (auto widget : ActiveWidgets)
	{
		if (WidgetBase == widget->GetClass())
		{
			return widget;
		}
	}
	return nullptr;
}

void ATNHMainHUD::RemoveCustomWidgetFromWidgetPool(AActor* _OwnningActor)
{
	if (_OwnningActor == nullptr)
		return;

	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	for (auto widget : ActiveWidgets)
	{
		auto BaseWidget = Cast<UBaseViewPortWidget>(widget);
		if (BaseWidget->GetWidgetParent() == _OwnningActor)
		{
			RemoveCustomWidgetFromWidgetPool(widget);
		}
	}
}

void ATNHMainHUD::RemoveCustomWidgetFromWidgetPool(FString Name)
{
	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	TSubclassOf<class UUserWidget>WidgetBase = GetCustomWidget(Name);
	for (auto widget : ActiveWidgets)
	{
		if (WidgetBase == widget->GetClass())
		{
			RemoveCustomWidgetFromWidgetPool(widget);
		}
	}
}

void ATNHMainHUD::RemoveAllCustomWidgetFromWidgetPool()
{
	for (UUserWidget* selectWidget : WidgetPool.GetActiveWidgets())
	{
		selectWidget->RemoveFromParent();
		WidgetPool.Release(selectWidget);
	}
}

UUserWidget* ATNHMainHUD::GetWidgetInHud(FString Name)
{
	if (Cur_ActiveWidget.Num() <= 0)
		return nullptr;

	for (auto var : Cur_ActiveWidget)
	{
		if (var->GetClass() == GetCustomWidget(Name))
			return var;
	}

	return nullptr;
}

void ATNHMainHUD::VisibleActivesWidgetpool()
{
	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	for (auto widget : ActiveWidgets)
	{
		widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATNHMainHUD::HiddenctivesWidgetpool()
{
	TArray<UUserWidget*>ActiveWidgets = WidgetPool.GetActiveWidgets();
	for (auto widget : ActiveWidgets)
	{
		widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

TSubclassOf<class UUserWidget> ATNHMainHUD::GetCustomWidget(FString Name)
{
	if (!IsValid(this))
		return nullptr;

	FCustomWidgetClass* CurrentTableData = CustomWidgetBaseTable->FindRow<FCustomWidgetClass>(FName(*Name), FString("NULL"), true);

	if (nullptr == CurrentTableData->CustomWidgetBase || nullptr == CurrentTableData)
		return nullptr;

	return CurrentTableData->CustomWidgetBase;
}

void ATNHMainHUD::SettingStateBar(float Data, float MaxData, bool Isbalance, UUserWidget* _stateBar)
{
	UWB_StateBar* stateBar = Cast<UWB_StateBar>(_stateBar);

	if (stateBar == nullptr)
		return;

	if (Isbalance)
	{
		if(stateBar->Dele_BalanceSetting.IsBound())
			stateBar->Dele_BalanceSetting.Execute(Data, MaxData);
	}
	else
	{
		if (stateBar->Dele_HPSetting.IsBound())
			stateBar->Dele_HPSetting.Execute(Data, MaxData);
	}
	stateBar->Super::ForceLayoutPrepass();
}


