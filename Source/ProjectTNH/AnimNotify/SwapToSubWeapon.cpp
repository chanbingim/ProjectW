// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapToSubWeapon.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/Weapon/WeaponBase.h"

void USwapToSubWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AAi_CharacterBase* AICharacter = Cast<AAi_CharacterBase>(MeshComp->GetOwner());

	if (AICharacter)
	{
		if (IsValid(AICharacter->subWeapon))
		{
			AICharacter->GetDidItHitComponent()->SkipStringFilter = "LeftHand";
			AICharacter->GetDidItHitComponent()->SetupVariables(AICharacter->subWeapon->GetItemMesh(), nullptr);
		}
	}
}

void USwapToSubWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AAi_CharacterBase* AICharacter = Cast<AAi_CharacterBase>(MeshComp->GetOwner());

	if (AICharacter)
	{
		if (IsValid(AICharacter->MainWeapon))
		{
			AICharacter->GetDidItHitComponent()->SkipStringFilter = "LeftHand";
			AICharacter->GetDidItHitComponent()->SetupVariables(AICharacter->MainWeapon->GetItemMesh(), nullptr);
		}
	}
}
