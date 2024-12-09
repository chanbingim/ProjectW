// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UAiCommander.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeleVoidHandle);

USTRUCT(BlueprintType)
struct F2DVectorArray
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> InnerArray;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTTNH_API UUAiCommander : public UWorldSubsystem
{
	GENERATED_BODY()

private :
	FTimerHandle UpdateTimer;

	UFUNCTION()
	void ClearDeadAi();

public:
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> WorldAllEnemyPawn;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> WorldAllTeamPawn;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> DeadAiPawnList;

protected:
	virtual void PostInitialize();
	virtual void Deinitialize();
public:
	UUAiCommander();

	UFUNCTION()
	void CustonInitialize();

	UFUNCTION()
	void AiChangedStateToDead(AActor* _deadPawn);

	UFUNCTION()
	void SettingNewMission(AActor* _Mission);

	UFUNCTION()
	void SetTargetOffsetPosition(AActor* _Target, int TeamNum);

	UFUNCTION()
	void CheckEnemyDeathCount();

	UFUNCTION()
	void AiTargetUpdateRate();

	FDeleVoidHandle FCheckFunction;
};
