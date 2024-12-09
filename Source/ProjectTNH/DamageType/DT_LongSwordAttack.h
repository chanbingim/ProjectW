// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "../DataTable/DT_AttackRow.h"
#include "DT_LongSwordAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_LongSwordAttack : public UDamageType
{
	GENERATED_BODY()
	
private:
	EReactionType DamageReactionType = EReactionType::ERT_LowDamage;

	FVector KnockbackVector;

	ECombatDirection Attack_Direction;

	float DamageAmount;

	float BalanceAmount;

public:
	void SetDamageReactionType(EReactionType _ReactType) { DamageReactionType = _ReactType; }
	EReactionType GetDamageReactionType() { return DamageReactionType; }

	void SetKnockbackVector(FVector _Amount) { KnockbackVector = _Amount; }
	FVector GetKnockbackVector() { return KnockbackVector; }

	void SetCombatDirection(ECombatDirection _AttackDir) { Attack_Direction = _AttackDir; }
	ECombatDirection GetCombatDirection() { return Attack_Direction; }

	void SetDamageAmount(float _Amount) { DamageAmount = _Amount; }
	float GetDamageAmount() { return DamageAmount; }

	void SetBalanceAmount(float _Amount) { BalanceAmount = _Amount; }
	float GetBalanceAmount() { return BalanceAmount; }
};
