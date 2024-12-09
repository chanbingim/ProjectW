// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "DT_AttackProperties.generated.h"

UENUM(BlueprintType)
enum class EAiAttackType : uint8
{
	NormalAttack UMETA(DisplayName = "NormalAttack"),
	ChargeAttack UMETA(DisplayName = "ChargeAttack"),
	Breaking UMETA(DisplayName = "Breaking"),
	Parring UMETA(DisplayName = "Parring"),
};

UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
	HitBreak UMETA(DisplayName = "HitBreak"),
	HitParring UMETA(DisplayName = "HitParring"),
};

USTRUCT(BlueprintType)
struct FDT_Attack : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	EAiAttackType T_Attack = EAiAttackType::NormalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	float BlockBalance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	float HitBalance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	float MissedBalance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	bool bIsDodgeable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	bool bIsUnBlockable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	bool bIsParriable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	bool bIsSuperArmo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	bool bIsMakeStagger = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	UAnimMontage* PreAttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	UAnimMontage* ChargeAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	UAnimMontage* AttackAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_Attack")
	float ChargeTime = 0.0f;

};
/**
 * 
 */
UCLASS()
class PROJECTTNH_API UDT_AttackProperties : public UDataTable
{
	GENERATED_BODY()
};
