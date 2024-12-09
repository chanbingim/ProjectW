// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseViewPortWidget.h"
#include "../Character/PawnType.h"
#include "WB_StateBar.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDele_Setting, float, Data1, float, Data2);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDele_NameSetting, FString, Data1);

class UProgressBar;
class UTextBlock;

USTRUCT(BlueprintType)
struct FStateOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProgressBar* HPBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProgressBar* AnimationHPBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProgressBar* BalanceBar = nullptr;
};

USTRUCT(BlueprintType)
struct FNameOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextBlock* NameBlock = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPawnType OwnningBlockPawnType = EPawnType::NormalSword;
};

UCLASS()
class PROJECTTNH_API UWB_StateBar : public UBaseViewPortWidget
{
	GENERATED_BODY()
private :
	UPROPERTY()
	float MaxHP;

	UPROPERTY()
	float CurrentHP;

	UPROPERTY()
	float Pre_AnimationHpData;

	UPROPERTY()
	float AnimationPlayRate = 0.0f;

	UPROPERTY()
	FTimerHandle AnimationTimerHandle;

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationPlaySpeed = 0.0f;

	FDele_Setting Dele_HPSetting;
	FDele_Setting Dele_BalanceSetting;
	FDele_NameSetting Dele_NameSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStateOption StateOption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNameOption> NameOption;

	UFUNCTION(BlueprintCallable)
	void SetHpState(float _CurrentHP, float _MaxHP);

	UFUNCTION(BlueprintCallable)
	void SetBalanceState(float CurrentBal, float MaxBal);

	UFUNCTION()
	void Play_AnimationBar(float _CurrentData, float _MaxData);

	UFUNCTION()
	void SetTextBlock(FString name);
};
