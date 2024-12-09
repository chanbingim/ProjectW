// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TNHGameInstance.generated.h"


class USaveDatamanagerClass;
class UGameMissionClass;

UCLASS()
class PROJECTTNH_API UTNHGameInstance : public UGameInstance
{
	GENERATED_BODY()

private :
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	USaveDatamanagerClass* SaveManager;

	UPROPERTY(VisibleAnywhere, Category = "Manager")
	UGameMissionClass* MissionManager;
protected :
	virtual void Init();
	virtual void Shutdown();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	TSubclassOf<USaveDatamanagerClass> subClassSaveManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	TSubclassOf<UGameMissionClass> subClassMissionManager = nullptr;
public :
	UFUNCTION()
	void InitializeHUD(AHUD* HUD);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* AttackDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* CharacterStatusDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* AnimationDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* ExecutionDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* ETCTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MediaPlayer")
	class UMediaPlayer* _mediaPlayer;

	
};
