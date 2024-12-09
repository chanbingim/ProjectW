// Fill out your copyright notice in the Description page of Project Settings.

#include "MinionController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "Ai_CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "../TargetSelectComponent.h"
#include "ProjectTNH/BoundBoxBase.h"
#include "ProjectTNH/AttackListComponents.h"
#include "NavigationSystem.h"
#include "../AiMoveComponents.h"
#include "../UAiCommander.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "Async/Async.h"
#include "Perception/AIPerceptionComponent.h"
#include "Navigation/PathFollowingComponent.h"


AMinionController::AMinionController()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMinionController::BeginPlay()
{
	Super::BeginPlay();
	



}

void AMinionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentSearchTime += DeltaTime;

	if (OwnningPawn->CurrentState == ECharacterState::Move)
		OwnningPawn->GetMoveComponents()->GetShereOverlapActors();

	if (CurrentSearchTime >= TargetSearchTime)
	{
		MinionReaction();
		CurrentSearchTime = 0;
	}
		
}

void AMinionController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InitMinionController();
	SetAiStatusData(OwnningPawn->GetPawnType());

	if (OwnningPawn->MissionObject != nullptr)
	{
		OwnningPawn->CurrentState = ECharacterState::Run;
		OwnningPawn->GetMoveComponents()->MovementAccordingToStatus();
	}
}

void AMinionController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AMinionController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if(OwnningPawn->CurrentState == ECharacterState::Dead)
		return;

	if (Result.IsSuccess())
	{
		// 이동이 성공적으로 완료되었을 때 실행할 코드
		switch (OwnningPawn->CurrentState)
		{
		case ECharacterState::Move :

			break;

		case ECharacterState::Run :
			OwnningPawn->SetCharacterWalkSpeed(OwnningPawn->Walkspeed);
			break;
		}

		OwnningPawn->CurrentState = ECharacterState::Idle;
	}
}

void AMinionController::PostRegisterAllComponents()
{
	Super::PostInitializeComponents();

	
}

void AMinionController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AMinionController::OnTargetDetected(const TArray<AActor*>& UpdatedActors)
{
	
}

void AMinionController::InitMinionController()
{
	if (nullptr != OwnningPawn)
	{
		if (OwnningPawn->OnDeaded.Contains(this, FName(TEXT("MinionDead"))) == false)
		{
			OwnningPawn->OnDeaded.AddDynamic(this, &ThisClass::MinionDead);
		}

		PropertiesData = AiSatusDataTable->FindRow<FAIStatusRow>(FName(OwnningPawn->GetPawnType()), "");

		SafetyDistance = OwnningPawn->GetMoveComponents()->GetSafeDistance();
		TargetComp = OwnningPawn->GetTargetSelectComponents();
	}
}

void AMinionController::SettingShieldItem()
{
	FString SocketName;

	if (LeftWeapon != nullptr)
		return;

	if (OwnningPawn->pawnType == EPawnType::NormalSword)
	{
		SocketName = "ShieldSocket";
		FTransform SpawnTransform = OwnningPawn->GetMesh()
			->GetSocketTransform(FName(SocketName), ERelativeTransformSpace::RTS_Component);
		switch (OwnningPawn->TeamNum)
		{
		case 1:
			LeftWeapon = GetWorld()->SpawnActor<AActor>(friendlyShield, SpawnTransform);
			break;

		case 2:
			LeftWeapon = GetWorld()->SpawnActor<AActor>(EnemyShield, SpawnTransform);
			break;
		}

		if (LeftWeapon == nullptr)
			return;
	}
	else if (OwnningPawn->pawnType == EPawnType::NormalArcher)
	{
		SocketName = "WeaponBow";
		FTransform SpawnTransform = OwnningPawn->GetMesh()
			->GetSocketTransform(FName(SocketName), ERelativeTransformSpace::RTS_Component);
		LeftWeapon = GetWorld()->SpawnActor<AActor>(MinionBow, SpawnTransform);

		if (LeftWeapon == nullptr)
			return;

		OwnningPawn->MainWeapon = Cast<AWeaponBase>(LeftWeapon);
	}
	
	LeftWeapon->AttachToComponent(OwnningPawn->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale, FName(SocketName));
	
}

void AMinionController::MinionDead()
{
	// Code
	if (OwnningPawn->CurrentState == ECharacterState::Dead)
	{
		StopMovement();

		OnUnPossess();
		GetWorld()->GetTimerManager().ClearTimer(MoveDelayHandle);
		OwnningPawn->DestroyMainWeapon();
		if(LeftWeapon != nullptr)
			LeftWeapon->Destroy();
		Destroy();
	}
}

void AMinionController::MinionReaction()
{
	auto TargetActor = OwnningPawn->TargetActor;
	SetFocus(TargetActor);

	if (TargetActor != nullptr && (OwnningPawn->CurrentState == ECharacterState::Idle
		|| OwnningPawn->CurrentState == ECharacterState::Move || OwnningPawn->CurrentState == ECharacterState::Run))
	{
		float Distance = OwnningPawn->GetDistanceTo(TargetActor);
		OwnningPawn->SetCharacterWalkSpeed(OwnningPawn->Walkspeed);

			if (!OwnningPawn->bIsAttackCoolTime &&
				Distance <= PropertiesData->AttackRange)
			{
				auto reactionTime = OwnningPawn->reactionTime;
				float randomReaction = FMath::FRandRange(0.1f, reactionTime);
				OwnningPawn->CurrentState = ECharacterState::Attack;
				OwnningPawn->bIsAttackCoolTime = true;

				if (!GetWorld()->GetTimerManager().IsTimerActive(AttackDelayHandle))
				{
					TWeakObjectPtr<AMinionController> WeakPtr = this;

					GetWorld()->GetTimerManager().SetTimer(AttackDelayHandle, FTimerDelegate::CreateLambda([WeakPtr]()
						{
							if (WeakPtr.IsValid())  // 객체 유효성 검사
							{
								auto TargetActor = WeakPtr->OwnningPawn->TargetActor;

								WeakPtr->OwnningPawn->AttackSelect(-2, TargetActor);
								WeakPtr->GetWorld()->GetTimerManager().ClearTimer(WeakPtr->AttackDelayHandle);
							}

							
						}), randomReaction, false);
				}
			}
			else if(ECharacterState::Attack != OwnningPawn->CurrentState)
			{
				OwnningPawn->CurrentState = ECharacterState::Move;
				OwnningPawn->GetMoveComponents()->MovementAccordingToStatus();
			}
	}
	else if (TargetActor == nullptr && OwnningPawn->CurrentState == ECharacterState::Idle)
	{
		float Distance = 0;
		if (nullptr != OwnningPawn->MissionObject)
		{
			auto MissionObj = OwnningPawn->MissionObject;
			if (MissionObj != nullptr)
				Distance = MissionObj->GetDistanceTo(OwnningPawn);

			if (Distance >= AreaOutDistance)
			{
				OwnningPawn->CurrentState = ECharacterState::Run;
				OwnningPawn->GetMoveComponents()->MovementAccordingToStatus();
			}
			else
				StopMovement();
		}
		else
			StopMovement();
	}
}