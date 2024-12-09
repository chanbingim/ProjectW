// Fill out your copyright notice in the Description page of Project Settings.


#include "TNHCharacterAnimInstance.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectTNH/Weapon/WeaponBase.h"

void UTNHCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<ATNHCharacter>(TryGetPawnOwner());

	if (OnMontageEnded.Contains(this, FName(TEXT("MontageEndResetState"))) == false)
	{
		OnMontageEnded.AddDynamic(this, &ThisClass::MontageEndResetState);
	}
}

void UTNHCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MyCharacter == nullptr)
	{
		MyCharacter = Cast<ATNHCharacter>(TryGetPawnOwner());
	}

	if (MyCharacter == nullptr) return;

	//Direction = UKismetAnimationLibrary::CalculateDirection(MyCharacter->GetVelocity(), MyCharacter->GetActorRotation());

	Forward = UKismetMathLibrary::Dot_VectorVector(MyCharacter->GetVelocity(), MyCharacter->GetActorForwardVector()) / 250.f;
	Side = UKismetMathLibrary::Dot_VectorVector(MyCharacter->GetVelocity(), MyCharacter->GetActorRightVector()) / 250.f;

	Direction = MyCharacter->CurrentDirection;

	switch (Direction)
	{
	case ECombatDirection::Left:
		Direction_PlayRate = -1.f;
		break;
	case ECombatDirection::Right:
		Direction_PlayRate = 1.f;
		break;
	}

	LButtonDown = MyCharacter->GetLButtonDown();
	RButtonDown = MyCharacter->GetRButtonDown();
	WheelDown = MyCharacter->GetWheelDown();

	bUseLHandIk = MyCharacter->GetUseHandIk();

	IsTargetExist = MyCharacter->GetTargetExist();

	if (MyCharacter->GetMainWeapon())
	{
		UMeshComponent* WeaponMesh = MyCharacter->GetMainWeapon()->GetItemMesh();

		if (WeaponMesh)
		{
			if (MyCharacter->CurrentDirection == ECombatDirection::Left)
			{
				LeftHandTransform = WeaponMesh->GetSocketTransform(FName(TEXT("LeftHandSocket_L")), ERelativeTransformSpace::RTS_Component);
			}
			else
			{
				LeftHandTransform = WeaponMesh->GetSocketTransform(FName(TEXT("LeftHandSocket_R")), ERelativeTransformSpace::RTS_Component);
			}
		}
	}
}

void UTNHCharacterAnimInstance::MontageEndResetState(UAnimMontage* _Montage, bool _bInterrupted)
{
	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(GetOwningActor());

	if(OwnerPawn->CurrentState == ECharacterState::Parrying)
		OwnerPawn->CurrentState = ECharacterState::Idle;
}

void UTNHCharacterAnimInstance::UpdateCharacterInfo_Implementation()
{

}
