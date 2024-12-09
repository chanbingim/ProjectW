// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMissionComponets.h"
#include "AI/UAiCommander.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"
#include "ProjectTNH/TNHGameInstance.h"
#include "Character/TNHCharacter.h"
#include "MissionObjectiveComponent.h"
#include "ProjectTNH/UI/HUD/TNHMainHUD.h"
#include "SaveDatamanagerClass.h"
#include "MediaPlayer.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "ActorBaseClass.h"

UGameMissionClass* UGameMissionClass::SingletonInstance = nullptr;

// Sets default values for this component's properties
UGameMissionClass::UGameMissionClass()
{
	


}

UGameMissionClass* UGameMissionClass::GetInstance()
{
	if (SingletonInstance != nullptr)
		return SingletonInstance;

	return nullptr;
}

void UGameMissionClass::Initialize()
{
	// 싱글톤 인스턴스 설정
	if (!SingletonInstance)
	{
		SingletonInstance = this;
		SingletonInstance->AddToRoot();
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGameMissionClass::OnLevelLoaded);
	}
	else
	{
		SingletonInstance->RemoveFromRoot();
		SingletonInstance->MarkPendingKill();
	}
}

void UGameMissionClass::ShutDown()
{
	if (SingletonInstance)
	{
		SingletonInstance->RemoveFromRoot();
		SingletonInstance->MarkPendingKill();
		SingletonInstance = nullptr;
	}
}

void UGameMissionClass::SettingData(FName LevelName)
{
	FString _LevelPreFix = GetWorld()->StreamingLevelsPrefix;
	FString result = LevelName.ToString().Mid(_LevelPreFix.Len());

	if (MissionDataTable.Num() > 0)
	{
		UDataTable** FoundDataTable = MissionDataTable.Find(FName(result));
		UDataTable* currentMission = FoundDataTable ? *FoundDataTable : nullptr;

		if (currentMission != nullptr)
		{
			FMissionData* _Data = currentMission->FindRow<FMissionData>(FName(FString::FromInt(CurrentMissionIndex)), "");
			TArray<FName> RowNames = currentMission->GetRowNames();
			MaxMissionIndex = RowNames.Num();

			if (_Data != nullptr)
			{
				MissionName = _Data->MissionName;
				MissionExplication = _Data->MissionExplication;
			}
			else
			{
				if (RowNames.Num() <= CurrentMissionIndex)
				{
					ClearGame();
				}
			}
		}
	}
}

FText UGameMissionClass::GetMissionName()
{
	if (SingletonInstance == nullptr)
		return FText::FromString("");

	if (MissionName.IsEmpty())
		return FText::FromString("");

	return FText::FromString(MissionName);
}
FText UGameMissionClass::GetMissionExplication()
{
	if (SingletonInstance == nullptr)
		return FText::FromString("");

	if (MissionName.IsEmpty())
		return FText::FromString("");

	return FText::FromString(MissionExplication);
}



void UGameMissionClass::OnLevelLoaded(UWorld* LoadedWorld)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Loadmap"));
	auto SaveManager = USaveDatamanagerClass::GetInstance();
	auto saveData = SaveManager->GetPlayerData();

	auto mainHud = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	mainHud->Init();

	if (SaveManager)
	{
		bool flag = SaveManager->bIsPlaycontinued;
		if (flag)
		{
			auto Player = Cast<ATNHCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

			CurrentMissionIndex = saveData.Mission_Index - 1;
			Player->Health = saveData.Player_Health;
			Player->balance = saveData.Player_Balance;
			Player->SetActorLocation(saveData.Player_Location);

			TArray<AActor*> AllActors;
			AllActors = GetActorsWithComponent();
			for (auto _actor : AllActors)
			{
				auto MissionActor = Cast<AActorBaseClass>(_actor);
				if (!MissionActor)
					return;

				auto MisstionComp = Cast<UMissionObjectiveComponent>(MissionActor->FindComponentByClass(UMissionObjectiveComponent::StaticClass()));
				if (MisstionComp)
				{
					if (MisstionComp->LevelMissionID < CurrentMissionIndex)
					{
						MissionActor->Destroy();
						continue;
					}
					else if(MisstionComp->LevelMissionID == CurrentMissionIndex)
					{
						MisstionComp->ClearMission();
						break;
					}
				}
			}

			SaveManager->bIsPlaycontinued = false;
		}
		else
		{
			SettingData(FName(GetWorld()->GetMapName()));
			Dele_NextMission.Broadcast();
		}
	}
}

TArray<AActor*> UGameMissionClass::GetActorsWithComponent()
{
	TArray<AActor*> AllActors;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActorBaseClass::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (Actor && Actor->FindComponentByClass(UMissionObjectiveComponent::StaticClass())) // 특정 컴포넌트가 있는지 확인
		{
			OutActors.Add(Actor);
		}
	}

	return OutActors;
}

void UGameMissionClass::InitNextMission()
{
	FString _LevelPreFix = GetWorld()->StreamingLevelsPrefix;
	FString result = GetWorld()->GetMapName().Mid(_LevelPreFix.Len());

	CurrentMissionIndex += 1;
	SettingData(FName(GetWorld()->GetMapName()));

	Dele_NextMission.Broadcast();
}

void UGameMissionClass::ClearGame()
{
	FString _LevelName = GetWorld()->GetMapName();
	FString Result = _LevelName.Mid(GetWorld()->StreamingLevelsPrefix.Len());

	if (CutSceneList.Num() > 0)
	{
		FCutSceneData* FoundDataTable = CutSceneList.Find(FName(Result));
		UTNHGameInstance* GameInstance = Cast<UTNHGameInstance>(UGameplayStatics::GetGameInstance(this));
		ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this,0)->GetHUD());

		UMediaPlayer* _mediaplayer = GameInstance->_mediaPlayer;

		if (mainHUD)
		{
			mainHUD->CustomWidgetAdd("CutScene");
		}

		if (_mediaplayer)
		{
			UGameplayStatics::SetGamePaused(this, true);
			_mediaplayer->OpenSource(FoundDataTable->SequenceAsset);
			Dele_StartCutScene.Broadcast();
			// onfinished 델리게이트 설정 (옵션)
			_mediaplayer->OnEndReached.AddDynamic(this, &ThisClass::OnSequenceFinished);
		}
	}
}

void UGameMissionClass::OnSequenceFinished()
{
	FString _LevelName = GetWorld()->GetMapName();
	FString Result = _LevelName.Mid(GetWorld()->StreamingLevelsPrefix.Len());

	if (CutSceneList.Num() > 0)
	{
		FCutSceneData* FoundDataTable = CutSceneList.Find(FName(Result));

		if (FoundDataTable != nullptr)
		{
			UGameplayStatics::SetGamePaused(this, false);
			Dele_ClearLevel.Broadcast(*FoundDataTable);
		}
	}
}

FMissionStatus UGameMissionClass::GetCurrentMissionIndex()
{
	FMissionStatus data;
	
	data.MissionCount = MaxMissionIndex;
	data.CurrentMissionIndex = CurrentMissionIndex;
	return data;
}
