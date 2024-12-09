// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomWidget_PlayerStateBarBase.generated.h"

class UCanvasPanel;

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UCustomWidget_PlayerStateBarBase : public UUserWidget
{
	GENERATED_BODY()
private :
	UPROPERTY(VisibleAnywhere)
	TArray<UUserWidget *>ActiveWidget;

protected :
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override ;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* MissionDataUI;

	UPROPERTY(BlueprintReadOnly , meta = (BindWidget))
	UUserWidget* LockOnWidget;

public :
	FORCEINLINE UCanvasPanel* GetMainCanvas() { return MainCanvas; }

	UFUNCTION()
	void VisbleLockOn(AActor* LockOnActor);

	UFUNCTION()
	void HiddenLockOn();

	UFUNCTION()
	void MissionUIVisible(ESlateVisibility visible);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerParried();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerGuarded();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerAttackStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerAttackEnded();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerAttackBlocked();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerHited();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLockOnStarted(AActor* _Target);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLockOnReleased(AActor* _Target);
};
