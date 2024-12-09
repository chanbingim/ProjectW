// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMissionTimeWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTTNH_API UUMissionTimeWidget : public UUserWidget
{
	GENERATED_BODY()
	
private :
	float CurrentTime;
	float _timeRate;


	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentTimeText;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeletaTime) override;

public :
	UFUNCTION()
	void Init(float _curTime, float _Rate);

	UFUNCTION()
	void UpdateWidget();
};
