// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSelectComponent.h"
#include "Ai_CharacterBase/Ai_CharacterBase.h"
#include "Ai_CharacterBase/AiBaseController.h"
#include "../Character/BaseCharacterClass.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"

// Sets default values for this component's properties
UTargetSelectComponent::UTargetSelectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTargetSelectComponent::SelectSingleTarget(AActor* _Target)
{
	auto OwnerPawn = Cast<ABaseCharacterClass>(Owner);
	auto TargetPawn = Cast<ABaseCharacterClass>(_Target);
	bool flag = false;

	HostileEntities.Empty();
	OwnerPawn->TargetActor = _Target;

	if (TargetPawn != nullptr)
	{
		auto Temppawn = Cast<ABaseCharacterClass>(TargetPawn->TargetActor);
		HostileEntities.Add(_Target);

		if(Temppawn != nullptr)
		flag = TargetPawn->pawnType >= EPawnType::EliteSword && Temppawn->pawnType == EPawnType::Player;

		if (TargetPawn->pawnType != EPawnType::Player && !flag)
			TargetPawn->TargetActor = OwnerPawn;

		TargetPawn->GetTargetSelectComponents()->ResetAllHostileData();
		TargetPawn->GetTargetSelectComponents()->AddHostileData(OwnerPawn);
	}
}

void UTargetSelectComponent::ResetAllHostileData()
{
	if(HostileEntities.Num() > 0)
		HostileEntities.Empty();
}

void UTargetSelectComponent::RemoveHostileData(AActor* _Target)
{
	if (HostileEntities.Num() > 0)
	{
		if (HostileEntities.Contains(_Target))
		{
			HostileEntities.Remove(_Target);
		}
	}

}