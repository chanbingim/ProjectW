// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseViewPortWidget.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "DirectionIndicator_Widget.generated.h"

UENUM(BlueprintType)
enum class EType : uint8
{
	Attack UMETA(DisplayName = "Attack"),
	Guard UMETA(DisplayName = "Guard"),
	None UMETA(DisplayName = "None"),
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDirectionIndicator_Widget : public UBaseViewPortWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDirWidgetColor(EPawnType PawnType, EType _Type, ECombatDirection _Direction);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnAttack(ECombatDirection _Direction, EPawnType _PawnType, float _TotalDuration);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EndAttack();

protected :
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeletaTime) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* Dir_Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* Dir_Right;


	
};
