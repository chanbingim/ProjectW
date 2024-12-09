// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTable/MissionDataTable.h"
#include "GameMissionComponets.generated.h"

USTRUCT(BlueprintType)
struct FCutSceneData
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMediaSource* SequenceAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName nextLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameModeUrl;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextMission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnlearMission, FCutSceneData , LevelData);

USTRUCT(BlueprintType)
struct FMissionStatus
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentMissionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MissionCount = 0;
};

USTRUCT(BlueprintType)
struct FMissionTargetPos
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AActorBaseClass*> LevelBoundBoxAry;
};

UCLASS(BlueprintAble, BlueprintType)
class PROJECTTNH_API UGameMissionClass : public UObject
{
	GENERATED_BODY()
private :
	// 싱글톤 인스턴스 저장
	static UGameMissionClass* SingletonInstance;

	UPROPERTY()
	FString MissionName;

	UPROPERTY()
	FString MissionExplication;

	UPROPERTY(VisibleAnywhere)
	int CurrentMissionIndex;

	UPROPERTY(VisibleAnywhere)
	int MaxMissionIndex;

public:	
	// Sets default values for this component's properties
	UGameMissionClass();

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> MissionActorAry;

protected:
	UPROPERTY(EditAnywhere)
	TMap<FName, UDataTable*> MissionDataTable;

	UPROPERTY(EditAnywhere)
	TMap<FName, FCutSceneData> CutSceneList;

	UPROPERTY(EditAnywhere)
	TMap<FName, FMissionTargetPos> MissionBoundBoxAry;

public:
	UFUNCTION(BlueprintCallable, Category = "Singleton")
	static UGameMissionClass* GetInstance();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void ShutDown();

	UFUNCTION(BlueprintCallable)
	FText GetMissionName();

	UFUNCTION(BlueprintCallable)
	FText GetMissionExplication();

	UFUNCTION(BlueprintCallable)
	void SettingData(FName LevelName);

	UFUNCTION()
	void OnLevelLoaded(UWorld* LoadedWorld);

	UFUNCTION()
	TArray<AActor *> GetActorsWithComponent();

	UFUNCTION()
	void InitNextMission();

	UFUNCTION()
	void ClearGame();

	UFUNCTION()
	void OnSequenceFinished();

	UFUNCTION(BlueprintCallable)
	FMissionStatus GetCurrentMissionIndex();

	UPROPERTY(BlueprintAssignable)
	FOnNextMission Dele_StartCutScene;

	UPROPERTY(BlueprintAssignable)
	FOnNextMission Dele_NextMission;

	UPROPERTY(BlueprintAssignable)
	FOnlearMission Dele_ClearLevel;
};
