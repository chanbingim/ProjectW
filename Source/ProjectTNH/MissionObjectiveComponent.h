// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/BaseCharacterClass.h"
#include "MissionObjectiveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, AActor*, Target);

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	TargetElimination UMETA(DisplayName = "적 처치"),
	TargetEliminationArea  UMETA(DisplayName = "일정수 이상적 처치"),
	AreaMovement UMETA(DisplayName = "이동목표"),
	SurvivalOrDefense UMETA(DisplayName = "생존"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTTNH_API UMissionObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	int DeadCount;

	UPROPERTY();
	FTimerHandle MissionHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CurrentMissionTime;

	UPROPERTY(EditAnywhere, Category = "MissionData", meta = (EditCondition = "ObjectMissionType == EMissionType::TargetEliminationArea", EditConditionHides))
	float EliminationCount;

	UPROPERTY();
	TArray<ABaseCharacterClass*> CursedCharacters;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Collision")
	class USphereComponent* SphereComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<AActor*> NextStageSpawners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<AActor*> CurrentStageSpawners;
public:
	// Sets default values for this component's properties
	UMissionObjectiveComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MissionData", meta = (EditCondition = "ObjectMissionType == EMissionType::TargetElimination", EditConditionHides))
	TArray<AActor*>	MissionTargetObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMissionType ObjectMissionType;

	UPROPERTY(EditAnywhere, Category = "MissionData")
	int LevelMissionID;

	UPROPERTY(EditAnywhere, Category = "MissionData", meta = (EditCondition = "ObjectMissionType == EMissionType::SurvivalOrDefense", EditConditionHides))
	float MissionTime;

	UPROPERTY(EditAnywhere, Category = "MissionData", meta = (EditCondition = "ObjectMissionType == EMissionType::SurvivalOrDefense", EditConditionHides))
	float TimeRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SphereRadius;

	UPROPERTY(EditAnywhere, Category = "MissionData")
	bool IsEnemyCursed;

	UPROPERTY(EditAnywhere, Category = "MissionData")
	float CursedRecoverTime;

	UFUNCTION(BlueprintCallable)
	void BindActorEliminationAreaFUNC();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

public:
	UFUNCTION()
	void CheckOverlapArea(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapMissionPoint(AActor* overlapActor);

	UFUNCTION()
	void SetMissionTime(AActor* overlapActor);

	UFUNCTION()
	void ClearMission();

	UFUNCTION()
	void TargetEliminationClearCheck();

	UFUNCTION()
	void ActivateNextStageSpawners();

	UFUNCTION(BlueprintCallable)
	void AddEliminationTarget(AActor* _addTarget);

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveCompleted testbind;
};
