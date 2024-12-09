// Fill out your copyright notice in the Description page of Project Settings.


#include "TNHPlayerCameraBehavior.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "Character/ALSCharacterMovementComponent.h"

void UTNHPlayerCameraBehavior::OnInit(ATNHCharacter* _Owner)
{
	TNHCharacter = _Owner;

	if (TNHCharacter)
	{
		TNHCharacter->OnNormalAttackStarted.AddUObject(this, &ThisClass::NormalAttack);
		TNHCharacter->OnCrushAttackStarted.AddUObject(this, &ThisClass::CrushAttack);
		TNHCharacter->OnHited.AddUObject(this, &ThisClass::Hited);
		TNHCharacter->OnGuardSuccessed.AddUObject(this, &ThisClass::GuardSuccessed);

		if (TNHCharacter->OnDeaded.Contains(this, FName(TEXT("Deaded"))) == false)
			TNHCharacter->OnDeaded.AddDynamic(this, &ThisClass::Deaded);

		TNHCharacter->OnTargetFinded.AddUObject(this, &ThisClass::TargetFinded);
		TNHCharacter->OnTargetLosted.AddUObject(this, &ThisClass::TargetLosted);
	}
}

void UTNHPlayerCameraBehavior::TargetFinded(AActor* _Target)
{
	MainTarget = _Target;

	ExistTarget = true;
}

void UTNHPlayerCameraBehavior::TargetLosted(AActor* _Target)
{
	MainTarget = nullptr;

	ExistTarget = false;
}

bool UTNHPlayerCameraBehavior::IsMoving()
{
	if (TNHCharacter)
	{
		return (TNHCharacter->GetCharacterMovement()->Velocity.IsNearlyZero() == false);
	}

	return false;
}
