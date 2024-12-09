// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ai_CharacterBase.h"
#include "Boss_CharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ABoss_CharacterBase : public AAi_CharacterBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

private:
	FTimerHandle ExecutionStunTimer;

protected:
	UPROPERTY(EditAnywhere)
	float ExecutionStunRecoveryTime = 2.f;

public:
	virtual void Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance) override;
	virtual void OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt) override;

	virtual void OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage) override;

	virtual UAnimMontage* GroggyReaction(AActor* _DamageCauser, int _Direction) override;

	virtual UAnimMontage* DeathReaction(AActor* _DamageCauser, int _Direction) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBossDead();

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecutionStart();

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecutionEnd();

	void CalculateKnockBackPosition();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ExecutionDamage = 100.f;
};
