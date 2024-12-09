// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_AnimationTable.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FAnimationTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> LowDamage_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> LowDamageDir_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> LowDamage_Reaction_Minion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> HighDamage_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> KnockBack_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> Guard_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	UAnimMontage* GuardBreak_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> ParryCounterHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> ExecutionTarget_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> Stun_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> Death;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	TArray<UAnimMontage*> Push_Reaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	UAnimMontage* ParryStun_Reaction = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* Roll = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> Dodge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* Guard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> Parry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> ParryCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* Crush_Attack = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> Execution;
};

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_AnimationTable : public UDataTable
{
	GENERATED_BODY()
	
};
