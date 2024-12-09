// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeParringState.h"
#include "../../AI/Ai_CharacterBase/Ai_CharacterController.h"
#include "../../AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "Components/WidgetComponent.h"
#include "ProjectTNH/UI/DirectionIndicator_Widget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectTNH/UI/HUD/TNHMainHUD.h"
#include "ProjectTNH/UI/CustomWidget_PlayerStateBarBase.h"
#include "Kismet/GameplayStatics.h"

UChangeParringState::UChangeParringState()
{
	NodeName = TEXT("ChangeState");
	Interval = 4.0f;
}

void UChangeParringState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto Character = Cast<AAi_CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	auto Controller = Cast<AAi_CharacterController>(Character->GetController());

	ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	UCustomWidget_PlayerStateBarBase* PlayerStateBar = Cast< UCustomWidget_PlayerStateBarBase>(GameMainHUD->GetWidgetInHud("PlayerStateBar"));

	if (Controller && !bIsCanCnageGuardDirection(Character))
	{
		Controller->ChangeParringState(true);
	}
}

bool UChangeParringState::bIsCanCnageGuardDirection(AAi_CharacterBase* _Character)
{
	if (_Character->CurrentState == ECharacterState::Guard ||
		_Character->CurrentState == ECharacterState::Idle )
		return false;

	return true;
}
