// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AiBaseController.h"
#include "Ai_CharacterController.generated.h"

UCLASS()
class PROJECTTNH_API AAi_CharacterController : public AAiBaseController
{
	GENERATED_BODY()

public :
	AAi_CharacterController(FObjectInitializer const& object_initializer);

private :
	UPROPERTY(EditAnywhere, Category = "AITree")
	class UBehaviorTree* BTAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "AITree")
	class UBlackboardData* BBAsset = nullptr;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

	UPROPERTY(EditAnywhere, Category = "AISatusData")
	UAnimMontage* ChangeDirectionMon = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PostRegisterAllComponents() override;
	virtual void PostInitializeComponents() override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

public :
	UFUNCTION()
	void OnTargetDetected(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void TargetRelase(AActor* actor);
public :
	static const FName Key_TargetEnemy;
	static const FName Key_LevelObject;
	static const FName Key_AttackCoolTime;
	static const FName Key_GuardCoolTime;
	static const FName Key_StrafeRange;
	static const FName Key_CurrentState;
	static const FName Key_EQSMoveLoc;

	UPROPERTY(VisibleAnywhere)
	bool bIsMove = false;

	UFUNCTION()
	int AttackSelectFUNC();

	UFUNCTION()
	int32 selectEvansionWay();

	virtual void DeadAiPawn(AActor* KillerPawn) override;

	UFUNCTION()
	void ChangeParringState(bool isEnter);
};
