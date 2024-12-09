// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss_CharacterBase.h"
#include "BossSecondPhase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ABossSecondPhase : public ABoss_CharacterBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

private:
	FTimerHandle ExecutionStunTimer;

public:
	virtual void Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance) override;
	virtual void OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int ExecutionCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ExecutionDeathCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ExecutionDamagePercent = 0.25f;

public:
	int GetExecutionCount() { return ExecutionCount; }
};
