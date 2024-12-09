// Fill out your copyright notice in the Description page of Project Settings.


#include "AiEnemySpawn.h"
#include "AI/Ai_CharacterBase/AiBaseController.h"
#include "AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "TNHGameModeBase.h"
#include "BoundBoxBase.h"
#include "GameObjectPool.h"
#include "ProjectTNH/ActorBaseClass.h"
#include "Kismet/GameplayStatics.h"
#include "AI/UAiCommander.h"

// Sets default values
AAiEnemySpawn::AAiEnemySpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAiEnemySpawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsActive)
	{
		spawnActorInstance(0);
	}
}

#if WITH_EDITOR
bool AAiEnemySpawn::CanEditChange(const FProperty* InProperty) const
{
	// If other logic prevents editing, we want to respect that
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Can we edit flower color?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AAiEnemySpawn, relationActor))
	{
		return ParentVal && spawneTpye == ESpanwerType::MissionTargetSpawner;
	}

	return ParentVal;
}
#endif

// Called every frame
void AAiEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAiEnemySpawn::spawnActorInstance(int index)
{
	if (index < 0 || DataAry.Num() <= index)
		return;
	else
		AryIndex = index;
	TimerCount = DataAry[AryIndex].spawnCount;

	TWeakObjectPtr<AAiEnemySpawn>WeakPtr = this;
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, FTimerDelegate::CreateLambda([WeakPtr]()
		{
			if (WeakPtr.IsValid())
			{
				if (WeakPtr->DataAry[WeakPtr->AryIndex].BaseClass == nullptr)
				{
					WeakPtr->GetWorld()->GetTimerManager().ClearTimer(WeakPtr->spawnTimerHandle);
					return;
				}

				if (WeakPtr->bIsCreatedAtOnce)
				{
					for (int i = 0; i < WeakPtr->TimerCount; i++)
					{
						WeakPtr->SpawnAIPawn();
					}
					WeakPtr->TimerCount = -1;
				}
				else
				{
					if (WeakPtr->TimerCount > 0)
					{
						WeakPtr->SpawnAIPawn();
						WeakPtr->TimerCount -= 1;
					}
				}

				if (WeakPtr->AryIndex < WeakPtr->DataAry.Num() && WeakPtr->TimerCount <= 0)
				{
					WeakPtr->AryIndex += 1;
					if (WeakPtr->AryIndex < WeakPtr->DataAry.Num())
					{
						WeakPtr->TimerCount = WeakPtr->DataAry[WeakPtr->AryIndex].spawnCount;  // 다음 인덱스의 스폰 카운트로 초기화
					}
					else
					{
						WeakPtr->GetWorld()->GetTimerManager().ClearTimer(WeakPtr->spawnTimerHandle);
						if (WeakPtr->DestoryType == ESpawnerDestoryType::SpawnEndDestory)
							WeakPtr->Destroy();
						return;
					}
				}
			}
		
		}), DataAry[AryIndex].spawndelayTime, true);
	
	
}

void AAiEnemySpawn::SpawnAIPawn()
{
	if (DataAry.Num() <= AryIndex)
		return;

	ATNHGameModeBase* BaseGameMode = Cast<ATNHGameModeBase>(UGameplayStatics::GetGameMode(this));
	spawnPawn = Cast<AAi_CharacterBase>(BaseGameMode->objectPool->GetOrCreateInstance(DataAry[AryIndex].BaseClass));
	if (spawnPawn == nullptr)
		return;
	else
	{
		spawnPawn->pawnType = DataAry[AryIndex].AiType;
		spawnPawn->TeamNum = DataAry[AryIndex].spawnAIteam;
		spawnPawn->SetActorLocation(GetActorLocation());
		if (DataAry[AryIndex].MissionActor.Num() > 0)
			spawnPawn->MissionObject = DataAry[AryIndex].MissionActor[0];
		spawnPawn->TeamNum = DataAry[AryIndex].spawnAIteam;
		if (spawnPawn->MainWeapon == nullptr)
		{
			spawnPawn->InitData();
			spawnPawn->AddObjectPoolAndCommander();
		}

		if (spawneTpye == ESpanwerType::MissionTargetSpawner && relationActor != nullptr)
		{
			if(spawnPawn->TeamNum != 1)
				BlueprintCall_AddMissionTarget();
		}

		auto AiController = Cast<AAiBaseController>(spawnPawn->GetController());
		if (AiController)
		{
			AiController->Possess(spawnPawn); // 데이터 세팅이 완료된 후 Possess 호출
		}
	}
}

void AAiEnemySpawn::ActiveEevnetActor()
{
	if(bIsActive)
		spawnActorInstance(0);
}
