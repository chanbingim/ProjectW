// Fill out your copyright notice in the Description page of Project Settings.


#include "DetachWeapon.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Weapon/WeaponBase.h"

void UDetachWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacterClass* OwnerPawn = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	if (OwnerPawn)
	{
		AWeaponBase* Weapon = OwnerPawn->GetMainWeapon();

		if (Weapon)
		{
			//Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//Weapon->GetItemMesh()->SetCollisionObjectType(ECC_PhysicsBody);
			//Weapon->GetItemMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//Weapon->GetItemMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			//Weapon->GetItemMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			//Weapon->GetItemMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
			//Weapon->GetItemMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
			//Weapon->GetItemMesh()->SetEnableGravity(true);
			//Weapon->SetLifeSpan(20.f);
		}
	}
}