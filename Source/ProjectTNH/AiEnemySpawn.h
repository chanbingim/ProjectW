// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBaseClass.h"
#include "Character/BaseCharacterClass.h"
#include "AiEnemySpawn.generated.h"

UENUM(BlueprintType)
enum class ESpawnerDestoryType : uint8
{
	SpawnEndDestory UMETA(DisplayName = "End Destory"),
	MissionClearDestory UMETA(DisplayName = "Mission Clear Destory"),
};

UENUM(BlueprintType)
enum class ESpanwerType : uint8
{
	DefaultSpawner UMETA(DisplayName = "DefaultSpawner"),
	MissionTargetSpawner UMETA(DisplayName = "MissionTargetSpawner"),
};

USTRUCT(BlueprintType)
struct FSpawnData
 {
	 GENERATED_BODY()

 public:
	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnData")
	 TSubclassOf<class UObject> BaseClass = nullptr;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnState")
	 EPawnType AiType = EPawnType::NormalSword;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "SpawnState")
	 int spawnCount = 0;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnState")
	 int spawnAIteam = 0;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnState")
	 TArray<class AActor*> MissionActor;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnState")
	 float spawndelayTime = 0.0f;
 };

UCLASS()
class PROJECTTNH_API AAiEnemySpawn : public AActorBaseClass
{
	GENERATED_BODY()
	
public:	
	AAiEnemySpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	bool bIsCreatedAtOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionSpawnData", meta = (EditCondition = "spawneTpye == ESpanwerType::MissionTargetSpawner", EditConditionHides))
	AActor* relationActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnData")
	TArray<FSpawnData> DataAry;

	UFUNCTION(BlueprintCallable)
	void spawnActorInstance(int index);

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintCall_AddMissionTarget();

	FTimerHandle spawnTimerHandle;

	UFUNCTION()
	void SpawnAIPawn();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnState")
	ESpawnerDestoryType DestoryType = ESpawnerDestoryType::MissionClearDestory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnType")
	ESpanwerType spawneTpye = ESpanwerType::DefaultSpawner;

	virtual void ActiveEevnetActor() override;

private :
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SpawnData", meta = (AllowPrivateAccess = "true"))
	class AAi_CharacterBase* spawnPawn = nullptr;

	int AryIndex = 0;
	int TimerCount = 0;
};