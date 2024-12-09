// Fill out your copyright notice in the Description page of Project Settings.


#include "OverrideCharacterState.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Character/StateMachine.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/ProjectTNH.h"

void UOverrideCharacterState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (nullptr == BaseCharacter || BaseCharacter->CurrentState == ECharacterState::Dead)
		return;

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (PlayerCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s"), *GetEnumDisplayName(OverrideState)));
		PlayerCharacter->FSM_PlayerChangeState(OverrideState);
	}
	else if (BaseCharacter)
	{
		//BeforeState = BaseCharacter->CurrentState;
		BaseCharacter->CurrentState = OverrideState;
	}
}

void UOverrideCharacterState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (nullptr == BaseCharacter || BaseCharacter->CurrentState == ECharacterState::Dead)
		return;

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (PlayerCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s"), *GetEnumDisplayName(OverrideState)));
		PlayerCharacter->FSM_PlayerChangeState(OverrideState);
	}
	else if (BaseCharacter)
	{
		//BeforeState = BaseCharacter->CurrentState;
		BaseCharacter->CurrentState = OverrideState;
	}
}

void UOverrideCharacterState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());
	if (nullptr == BaseCharacter || BaseCharacter->CurrentState == ECharacterState::Dead)
		return;

	ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(MeshComp->GetOwner());

	if (PlayerCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s"), *GetEnumDisplayName(EndState)));
		PlayerCharacter->FSM_PlayerChangeState(EndState);
	}
	else if (BaseCharacter)
	{
		//BeforeState = BaseCharacter->CurrentState;
		BaseCharacter->CurrentState = EndState;
	}

	//ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(MeshComp->GetOwner());

	//if (BaseCharacter)
	//{
	//	BaseCharacter->CurrentState = BeforeState;
	//}
}
