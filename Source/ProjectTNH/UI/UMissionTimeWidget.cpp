// Fill out your copyright notice in the Description page of Project Settings.


#include "UMissionTimeWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UUMissionTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUMissionTimeWidget::NativeTick(const FGeometry& MyGeometry, float DeletaTime)
{
	Super::NativeTick(MyGeometry, DeletaTime);

	CurrentTime -= DeletaTime;
	UpdateWidget();
}

void UUMissionTimeWidget::Init(float _curTime, float _Rate)
{
	CurrentTime = _curTime;
	_timeRate = _Rate;

	FText _text = FText::FromString(FString::Printf(TEXT("%.2f"), CurrentTime));
	CurrentTimeText->SetText(_text);
}

void UUMissionTimeWidget::UpdateWidget()
{
	if (CurrentTime >= 0)
	{
		FText _text = FText::FromString(FString::Printf(TEXT("%.2f"), CurrentTime));
		CurrentTimeText->SetText(_text);
	}
	else
	{
		//this->RemoveFromParent();
	}
}
