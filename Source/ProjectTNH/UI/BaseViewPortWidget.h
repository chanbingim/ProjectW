// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseViewPortWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UBaseViewPortWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	void SetWidgetOwnningActor(AActor* _Parent) { OwnerActor = _Parent; };
	FORCEINLINE AActor* GetWidgetParent() { return OwnerActor; }
	FORCEINLINE void SetWidgetSize(FVector2D _Size) { WidgetSize = _Size; }
	FORCEINLINE FVector2D GetWidgetSize() { return WidgetSize; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere)
	class AActor* OwnerActor = nullptr;

	UPROPERTY(VisibleAnywhere)
	FVector2D WidgetSize = FVector2D(0,0);

};
