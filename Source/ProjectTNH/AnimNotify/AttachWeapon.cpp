// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachWeapon.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Weapon/WeaponBase.h"


void UAttachWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ATNHCharacter* OwnerPawn = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (OwnerPawn)
	{
		AWeaponBase* Weapon = OwnerPawn->GetMainWeapon();

		if (Weapon)
		{
			if (bLeftHand)
			{
				Weapon->AttachWeapon(FName("SwordHoldSocket"));
				OwnerPawn->IsNonCombatMode = true;
			}
			else
			{
				Weapon->AttachWeapon(FName("LongSwordHandSocket"));
				OwnerPawn->IsNonCombatMode = false;
			}
		}
	}
}
