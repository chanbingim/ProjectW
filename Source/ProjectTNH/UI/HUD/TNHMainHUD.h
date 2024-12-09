// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidgetPool.h"
#include "TNHMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ATNHMainHUD : public AHUD
{
	GENERATED_BODY()

protected: 
	virtual void BeginPlay() override;
	virtual void Tick(float DeletaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "WIDGETCLASS")
	class UDataTable* CustomWidgetBaseTable = nullptr;

public :
	ATNHMainHUD();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WidgetData")
	TArray<UUserWidget*> Cur_ActiveWidget;
	
	UFUNCTION()
	UUserWidget* CustomWidgetAdd(FString Name);

	UFUNCTION()
	void CustomWidgetRemove(FString Name);

	UFUNCTION()
	void Init();

	UFUNCTION()
	UUserWidget* AddCustomWidgetFromWidgetPool(FString Name);

	void RemoveCustomWidgetFromWidgetPool(UUserWidget* removeWidget);

	UUserWidget* FindCustomWidgetFromWidgetPool(AActor* _OwnningActor);

	UUserWidget* FindCustomWidgetFromWidgetPool(FString Name);

	void RemoveCustomWidgetFromWidgetPool(AActor* _OwnningActor);

	void RemoveCustomWidgetFromWidgetPool(FString Name);

	UFUNCTION()
	void RemoveAllCustomWidgetFromWidgetPool();

	UFUNCTION()
	UUserWidget* GetWidgetInHud(FString Name);

	UFUNCTION()
	void VisibleActivesWidgetpool();

	UFUNCTION()
	void HiddenctivesWidgetpool();

	UFUNCTION()
	TSubclassOf<class UUserWidget> GetCustomWidget(FString Name);

	UFUNCTION()
	void SettingStateBar(float Data, float MaxData, bool Isbalance, UUserWidget* _stateBar);

	UPROPERTY(VisibleAnywhere)
	TArray<UUserWidget*> PoolActiveWidget;

	FUserWidgetPool WidgetPool;
};
