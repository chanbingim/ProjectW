// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar_Widget.generated.h"


class ATNHCharacter;
/**
 * 
 */
UCLASS()
class PROJECTTNH_API UHPBar_Widget : public UUserWidget
{
	GENERATED_BODY()
	
	friend ATNHCharacter;
private:
	ATNHCharacter* OwnerCharacter;

public:
	void BindCustomDelegate();

private:
	UFUNCTION()
	void UpdateHP();

	UFUNCTION()
	void UpdateStamina();

private:
	UPROPERTY(Meta=(BindWidget))
	class UProgressBar* HP;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* AnimationBar;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* Stamina;
};
