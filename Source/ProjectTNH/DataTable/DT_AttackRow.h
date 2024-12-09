// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "DT_AttackRow.generated.h"

UENUM(BlueprintType)
enum class EReactionType : uint8
{
	ERT_LowDamage UMETA(DisplayName = "LowDamage"),
	ERT_HighDamage UMETA(DisplayName = "HighDamage"),
	ERT_KnockBack UMETA(DisplayName = "KnockBack"),
	ERT_GuardBreak UMETA(DisplayName = "GuardBreak"),
	ERT_Execution UMETA(DisplayName = "Execution"),
	ERT_Stun UMETA(DisplayName = "Stun"),
	ERT_None UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FAttackRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float BalanceDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float BlockBalance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float ParryBalance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	ECombatDirection AttackDirection = ECombatDirection::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	UAnimMontage* AttackAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	EReactionType ReactionType = EReactionType::ERT_LowDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float DiscountMoveAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float PostComboStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float HitReactPlaySecond = 0.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_AttackRow : public UDataTable
{
	GENERATED_BODY()
	
};
