// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionObjectiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "GameMissionComponets.h"
#include "ActorBaseClass.h"
#include "Character/BaseCharacterClass.h"
#include "AI/UAiCommander.h"
#include "Character/TNHCharacter.h"
#include "UI/UMissionTimeWidget.h"
#include "UI/HUD/TNHMainHUD.h"
#include "TNHGameModeBase.h"
#include "AiEnemySpawn.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UMissionObjectiveComponent::UMissionObjectiveComponent()
{
	SphereComponents = CreateDefaultSubobject<USphereComponent>("OverlapArea");
}

// Called when the game starts
void UMissionObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();

	AActorBaseClass* OwnerActor = Cast<AActorBaseClass>(GetOwner());;
	USceneComponent* RootComp = Cast<USceneComponent>(OwnerActor->GetRootComponent());

	SetActive(OwnerActor->GetActorActive());
		
	if (ObjectMissionType <= EMissionType::TargetEliminationArea)
	{
		if (ObjectMissionType == EMissionType::TargetEliminationArea)
		{
			auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();

			if (Commander->FCheckFunction.Contains(this, FName(TEXT("TargetEliminationClearCheck"))) == false)
			{
				Commander->FCheckFunction.AddDynamic(this, &ThisClass::TargetEliminationClearCheck);
			}
		}
		else
		{
			if (MissionTargetObject.Num() <= 0)
				return;

			for (auto _Target : MissionTargetObject)
			{
				auto TargetActor = Cast<AAi_CharacterBase>(_Target);

				if (TargetActor->OnDeaded.Contains(this, FName(TEXT("TargetEliminationClearCheck"))) == false)
				{
					TargetActor->OnDeaded.AddDynamic(this, &ThisClass::TargetEliminationClearCheck);
				}
			}
		}
	}
	else
	{
		if (SphereComponents->OnComponentBeginOverlap.Contains(this, FName(TEXT("CheckOverlapArea"))) == false)
		{
			SphereComponents->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::CheckOverlapArea);
		}
		SphereComponents->SetRelativeLocation(RootComp->GetRelativeLocation());
		SphereComponents->SetCollisionProfileName("OnlyPawnCheck");
		if (OwnerActor->GetActorActive())
			SphereComponents->SetSphereRadius(SphereRadius);
		else
		{
			SphereComponents->SetSphereRadius(0);
		}
		SphereComponents->bHiddenInGame = false ;
	}
	
	if (NextStageSpawners.Num() > 0)
	{
		for (auto _Actor : NextStageSpawners)
		{
			//연동된 모든 스포너 비활성화 후
			//미션을 깨면 활성화 시켜줄거임
			auto UnableActor = Cast<AActorBaseClass>(_Actor);
			if (UnableActor != nullptr)
			{
				UnableActor->SetActorActive(false);
			}
		}
	}
}

#if WITH_EDITOR
bool UMissionObjectiveComponent::CanEditChange(const FProperty* InProperty) const
{
	// If other logic prevents editing, we want to respect that
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Can we edit flower color?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UMissionObjectiveComponent, MissionTime))
	{
		return ParentVal && ObjectMissionType == EMissionType::SurvivalOrDefense;
	}

	// Can we edit food amount?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UMissionObjectiveComponent, EliminationCount))
	{
		return ParentVal && ObjectMissionType == EMissionType::TargetEliminationArea;
	}

	// Can we edit poison amount?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UMissionObjectiveComponent, MissionTargetObject))
	{
		return ParentVal && ObjectMissionType == EMissionType::TargetElimination;
	}
	
	return ParentVal;
}
#endif

void UMissionObjectiveComponent::CheckOverlapArea(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	switch (ObjectMissionType)
	{
	case EMissionType::AreaMovement:
		OverlapMissionPoint(OtherActor);
		break;
	case EMissionType::SurvivalOrDefense:
		SetMissionTime(OtherActor);

		break;
	default :
		break;
	}
}

void UMissionObjectiveComponent::OverlapMissionPoint(AActor* overlapActor)
{
	auto Player = Cast<ABaseCharacterClass>(overlapActor);
	auto Owner = Cast< AActorBaseClass>(GetOwner());
	if (Player == nullptr || Player->pawnType != EPawnType::Player)
		return;

	if (Owner->GetActorActive())
	{
		//미션 클리어했다는 신호
		ClearMission();
	}
}

void UMissionObjectiveComponent::SetMissionTime(AActor* overlapActor)
{
	auto Owner = Cast< AActorBaseClass>(GetOwner());

	auto Player = Cast<ABaseCharacterClass>(overlapActor);
	if (Player == nullptr || Player->pawnType != EPawnType::Player|| !Owner->GetActorActive())
		return;

	//미션 시작
	CurrentMissionTime = MissionTime;
	SphereComponents->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	auto currentHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	auto TimerWidget = Cast<UUMissionTimeWidget>(currentHUD->CustomWidgetAdd("TimerWidget"));
	TimerWidget->Init(CurrentMissionTime, TimeRate);

	GetWorld()->GetTimerManager().SetTimer(MissionHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (!IsValid(this))  // 객체 유효성 검사
			{
				return;  // 객체가 유효하지 않으면 실행 중단
			}

			if (CurrentMissionTime <= 0.0f)
			{
				auto currentHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
				currentHUD->CustomWidgetRemove("TimerWidget");

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Clear"));
				ClearMission();
				GetWorld()->GetTimerManager().ClearTimer(MissionHandle);
			}
			else
				CurrentMissionTime -= TimeRate;
			
		}), TimeRate, true);

}

void UMissionObjectiveComponent::BindActorEliminationAreaFUNC()
{
	
}

void UMissionObjectiveComponent::ClearMission()
{
	if (IsEnemyCursed)
	{
		auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();
		for (AActor* a : Commander->WorldAllEnemyPawn)
		{
			ABaseCharacterClass* Character = Cast<ABaseCharacterClass>(a);
			if (Character)
			{
				if (Character->pawnType == EPawnType::NormalSword || Character->pawnType == EPawnType::NormalArcher)
				{
					CursedCharacters.Add(Character);
					Character->Health = 1.f;
				}
			}
		}
	}
	
	//다음 목표
	UGameMissionClass::GetInstance()->InitNextMission();

	for (auto DestoryActor : CurrentStageSpawners)
	{
		auto Spawner = Cast<AAiEnemySpawn>(DestoryActor);
		GetWorld()->GetTimerManager().ClearTimer(Spawner->spawnTimerHandle);
		Spawner->Destroy();
	}

	if (ObjectMissionType == EMissionType::TargetEliminationArea)
	{
		auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();
		Commander->FCheckFunction.RemoveDynamic(this, &UMissionObjectiveComponent::TargetEliminationClearCheck);
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UMissionObjectiveComponent::ActivateNextStageSpawners);
}

void UMissionObjectiveComponent::TargetEliminationClearCheck()
{
	auto Owner = Cast< AActorBaseClass>(GetOwner());
	if (Owner->GetActorActive())
	{
		DeadCount += 1;

		if (MissionTargetObject.Num() > 0)
		{
			if (MissionTargetObject.Num() <= DeadCount &&
				ObjectMissionType == EMissionType::TargetElimination)
			{
				ClearMission();
				return;
			}
		}
		else if (DeadCount >= EliminationCount &&
			ObjectMissionType == EMissionType::TargetEliminationArea)
		{
			ClearMission();
			return;
		}
	}
}

void UMissionObjectiveComponent::ActivateNextStageSpawners()
{
	if (NextStageSpawners.Num() > 0)
	{
		for (auto _Actor : NextStageSpawners)
		{
			//연동된 모든 스포너 및 미션 활성화
			auto UnableActor = Cast<AActorBaseClass>(_Actor);
			if (UnableActor != nullptr)
			{
				UnableActor->SetActorActive(true);
				UnableActor->ActiveEevnetActor();
			}
		}
	}
	GetOwner()->Destroy();
}

void UMissionObjectiveComponent::AddEliminationTarget(AActor* _addTarget)
{
	auto targetPawn = Cast<ABaseCharacterClass>(_addTarget);
	auto Owner = Cast< AActorBaseClass>(GetOwner());

	if (targetPawn == nullptr || Owner == nullptr)
		return;

	targetPawn->OnDeaded.AddDynamic(this, &ThisClass::TargetEliminationClearCheck);
	MissionTargetObject.Add(targetPawn);

	if (Owner->GetActorActive())
	{
		testbind.Broadcast(_addTarget);
	}
}
