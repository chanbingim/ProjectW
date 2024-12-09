// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "AiCharacter_AnimBase.generated.h"

UCLASS()
class PROJECTTNH_API UAiCharacter_AnimBase : public UAnimInstance
{
	GENERATED_BODY()

private:
	class AAi_CharacterBase* OwnningAI = nullptr;

	float UpdateRotationTime;

	UFUNCTION()
	void CheckGuardDir();

	UFUNCTION()
	void CheckLookAtDir();

protected :
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuardDirection")
	bool LeftGuard = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Direction")
	FRotator LookDir;

	UFUNCTION()
	void BlendOutAnimation(UAnimMontage* animMontage, bool bInterrupted);
};
