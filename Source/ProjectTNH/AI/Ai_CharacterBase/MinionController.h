// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AiBaseController.h"
#include "MinionController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API AMinionController : public AAiBaseController
{
	GENERATED_BODY()

private :
	FTimerHandle MoveDelayHandle;
	FVector MoveAblePos;
	FTimerHandle AttackDelayHandle;

	class UTargetSelectComponent* TargetComp = nullptr;

	UPROPERTY()
	AActor* LeftWeapon = nullptr;

	UPROPERTY(EditAnywhere)
	float MinionAttackProbability;

	UPROPERTY(EditAnywhere)
	float MinionDectectionRadius = 200.f;

	UPROPERTY(EditAnywhere)
	int NumberOfPoints = 5;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> PawnChannel;

protected :
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	virtual void PostRegisterAllComponents() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnTargetDetected(const TArray<AActor*>& UpdatedActors);
	
public :
	AMinionController();

	UPROPERTY(EditAnywhere)
	float AreaOutDistance = 1000.0f;

	UPROPERTY(EditAnywhere)
	float TargetLostDistance = 1000.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> EnemyShield;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> friendlyShield;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> MinionBow;

	UFUNCTION()
	void InitMinionController();

	UFUNCTION(BlueprintCallable)
	void SettingShieldItem();

	UFUNCTION()
	void MinionDead();

	UFUNCTION()
	void MinionReaction();
};
