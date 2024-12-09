// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AiMoveComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UAiMoveComponents : public UActorComponent
{
	GENERATED_BODY()
private :
	//컴포넌트를 소유한 엑터
	UPROPERTY(VisibleAnywhere)
	AActor* OwnerActor = nullptr;

	//이동시 지연 시간
	UPROPERTY(EditAnywhere)
	float MoveDelayTime = 1.0f;

	//미니언 탐지거리
	UPROPERTY(EditAnywhere)
	float MinionDectectionRadius = 100.0f;

	//미니언 탐지거리 오프셋
	UPROPERTY(EditAnywhere)
	float MinionDectectionRadiusOffSet = 20.0f;

	UPROPERTY(EditAnywhere)
	int32 NumberOfPointArea = 5;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> AryMovePos;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> OverlapClosestActor;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> CheckObject;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere)
	float OverlapRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float _SafetyRange = 30.f;

	UPROPERTY(EditAnywhere)
	float DesiredSeparationDistance = 70.f;

	UFUNCTION()
	bool MoveToTargetPoint(FVector _TargetPoint);
	FTimerHandle MoveDelayHandle;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:	
	// Sets default values for this component's properties
	UAiMoveComponents();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Borides")
	float SeparationStrength = 20.f;

	UFUNCTION()
	void InitializeMoveComponent();

	UFUNCTION()
	void SearchAiMoveArea();

	UFUNCTION()
	void MovementAccordingToStatus();

	UFUNCTION(Blueprintcallable)
	FORCENOINLINE void SetOwnerActor(AActor* _Pawn) { OwnerActor = _Pawn; }

	UFUNCTION(Blueprintcallable)
	FORCENOINLINE AActor* GetOwnerActor() { return OwnerActor; }
	
	UFUNCTION(Blueprintcallable)
	FORCENOINLINE float GetSafeDistance() { return _SafetyRange; }

	UFUNCTION(Blueprintcallable)
	void ClearComponent();

	UFUNCTION()
	void GetShereOverlapActors();
};
