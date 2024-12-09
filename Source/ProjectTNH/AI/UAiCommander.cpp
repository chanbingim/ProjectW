// Fill out your copyright notice in the Description page of Project Settings.


#include "UAiCommander.h"
#include "Kismet/GameplayStatics.h"
#include "AiMoveComponents.h"
#include "../TNHGameModeBase.h"
#include "Ai_CharacterBase/Ai_CharacterBase.h"
#include "TargetSelectComponent.h"
#include "Ai_CharacterBase/AiBaseController.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"
#include "ProjectTNH/TNHGameModeBase.h"
#include "../Character/BaseCharacterClass.h"
#include "DrawDebugHelpers.h"

// Sets default values
UUAiCommander::UUAiCommander()
{


}

void UUAiCommander::CustonInitialize()
{
	auto TNHGameMode = Cast<ATNHGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (TNHGameMode != nullptr)
	{
		float timerRate = TNHGameMode->UpdateAiRate;

		if (!GetWorld()->GetTimerManager().IsTimerActive(UpdateTimer))
			GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &ThisClass::AiTargetUpdateRate, timerRate, true);
	}
}

void UUAiCommander::PostInitialize()
{
	Super::PostInitialize();

	
}

void UUAiCommander::Deinitialize()
{
	Super::Deinitialize();

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UUAiCommander::ClearDeadAi()
{
	auto DestoryAry = DeadAiPawnList;

	for (auto _Actor : DestoryAry)
	{
		bool flag = UActorUtiliyCompoents::IsActorInPlayerView(_Actor);
		if (!flag)
		{
			_Actor->Destroy();
			DeadAiPawnList.Remove(_Actor);
			auto GameMode = Cast<ATNHGameModeBase>(UGameplayStatics::GetGameMode(this));
			GameMode->RemoveActorsToPool(_Actor);
		}
	}
}

void UUAiCommander::AiChangedStateToDead(AActor* _deadPawn)
{
	if (_deadPawn == nullptr)
		return;

	if (WorldAllEnemyPawn.Num() > 0 &&
		WorldAllEnemyPawn.Contains(_deadPawn))
	{
		//ai�� �׾����� ���� �༮�� ��ü �迭�� �ִ� ����
		WorldAllEnemyPawn.Remove(_deadPawn);
		DeadAiPawnList.Add(_deadPawn);

		if(FCheckFunction.IsBound())
			FCheckFunction.Broadcast();
	}
	else if (WorldAllTeamPawn.Num() > 0 &&
		WorldAllTeamPawn.Contains(_deadPawn))
	{
		WorldAllTeamPawn.Remove(_deadPawn);
		DeadAiPawnList.Add(_deadPawn);
	}

	if (DeadAiPawnList.Num() >= 10)
	{
		ClearDeadAi();
	}
}

void UUAiCommander::SettingNewMission(AActor* _Mission)
{
	if (WorldAllEnemyPawn.Num() > 0)
	{
		for (auto _Pawn : WorldAllEnemyPawn)
		{
			Cast<ABaseCharacterClass>(_Pawn)->MissionObject = _Mission;
		}
	}
	
	if (WorldAllTeamPawn.Num() > 0)
	{
		for (auto _Pawn : WorldAllTeamPawn)
		{
			Cast<ABaseCharacterClass>(_Pawn)->MissionObject = _Mission;
		}
	}
}

void UUAiCommander::SetTargetOffsetPosition(AActor* _Target, int TeamNum)
{
	float MaxOffsetX = 50.0f;
	float MaxOffsetY = 50.0f;

	if (_Target == nullptr) return;

	TArray<AActor*> SearchActorAry;
	float offsetRadius = Cast<ATNHGameModeBase>(UGameplayStatics::GetGameMode(this))
		->GetLockOnBattlefieldOffset();

	SearchActorAry.Append(WorldAllEnemyPawn);
	SearchActorAry.Append(WorldAllTeamPawn);

	if (SearchActorAry.Num() <= 0)
		return;

	auto _mTarget = Cast<ABaseCharacterClass>(_Target);
	if (nullptr != _mTarget)
	{
		auto _TargetLockOnActor = _mTarget->GetTargetActor();
		if (_TargetLockOnActor == nullptr)
			return;

		SearchActorAry.Remove(_TargetLockOnActor);
		FVector HalfLocation = (_TargetLockOnActor->GetActorLocation() + _mTarget->GetActorLocation()) * 0.5f;
		for (auto _CloserActor : SearchActorAry)
		{
			if (_CloserActor == nullptr)
				continue;

			if (_CloserActor->GetName() == _TargetLockOnActor->GetName())
				continue;

			FVector v2 = _CloserActor->GetActorLocation();
			if (FVector::Distance(HalfLocation, v2) <= offsetRadius)
			{
				// �߽������κ��� ���� ��ġ������ ���� ���� ���
				FVector Direction = (v2 - HalfLocation).GetSafeNormal();
				// �ݰ� ������ �о ���ο� ��ġ ���
				FVector NewLocation = HalfLocation + Direction * offsetRadius;

				// �� ��ġ�� �̵�
				auto _mCloserActor = Cast<ABaseCharacterClass>(_CloserActor);
				if (_mCloserActor->CurrentState > ECharacterState::Run)
					return;

				auto _CloserActController = Cast<AAIController>(_CloserActor->GetInstigatorController());

				if (_mCloserActor->TargetActor != nullptr && _CloserActController != nullptr)
				{
					_CloserActController->SetFocus(_mCloserActor->TargetActor);

					float OffsetX = FMath::RandRange(-MaxOffsetX, MaxOffsetX);
					float OffsetY = FMath::RandRange(-MaxOffsetY, MaxOffsetY);

					NewLocation = FVector(OffsetX + NewLocation.X, OffsetY + NewLocation.Y, NewLocation.Z);
					_CloserActController->MoveToLocation(NewLocation);
				}
			}
		}
	}
}

void UUAiCommander::CheckEnemyDeathCount()
{ }

void UUAiCommander::AiTargetUpdateRate()
{
	TArray<AActor*> AllPawn, SortingData, selectData;
	if(WorldAllEnemyPawn.Num() > 0)
		AllPawn.Append(WorldAllEnemyPawn);

	if (WorldAllTeamPawn.Num() > 0)
		AllPawn.Append(WorldAllTeamPawn);

	if (AllPawn.Num() <= 0)
		return;

	for (auto _SearchPawn : AllPawn)
	{
		auto searchAI = Cast<ABaseCharacterClass>(_SearchPawn);
		if (nullptr == searchAI)
			continue;

		if (selectData.Num() > 0)
		{
			if (selectData.Contains(searchAI))
				continue;
		}

		if (searchAI->pawnType >= EPawnType::EliteSword)
		{
			auto _TargetActor = Cast<ABaseCharacterClass>(searchAI->TargetActor);
			if (_TargetActor != nullptr && _TargetActor->pawnType == EPawnType::Player)
			{
				selectData.Add(_TargetActor);
				continue;
			}
		}

		if (searchAI->TeamNum == 1) //�Ʊ�
			SortingData = WorldAllEnemyPawn;
		else //����
		{
			SortingData = WorldAllTeamPawn;
			SortingData.Add(UGameplayStatics::GetPlayerCharacter(this, 0));
		}

		AActor* ClosestTarget = nullptr;
		float MinDistance = FLT_MAX;

		for (auto _Target : SortingData)
		{
			if (_Target == nullptr)
				return;

			//Ÿ�� ���� ����
			float dis = FVector::Distance(_SearchPawn->GetActorLocation(), _Target->GetActorLocation());
			if (dis <= searchAI->SearchDistance && dis < MinDistance)
			{
				ClosestTarget = _Target;
				MinDistance = dis;
			}
		}

		// 1��1 Ÿ���� ����
		searchAI->GetTargetSelectComponents()->SelectSingleTarget(ClosestTarget);
		if (nullptr != ClosestTarget)
		{
			if (AllPawn.Contains(ClosestTarget))
				selectData.Add(ClosestTarget);
		}
	}
}
