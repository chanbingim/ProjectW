// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveDatamanagerClass.h"
#include "Kismet/GameplayStatics.h"
#include "Character/BaseCharacterClass.h"
#include "GameMissionComponets.h"

USaveDatamanagerClass* USaveDatamanagerClass::Instance = nullptr;

USaveDatamanagerClass* USaveDatamanagerClass::GetInstance()
{
	if (Instance)
		return Instance;

	return nullptr;
}

USaveDatamanagerClass::USaveDatamanagerClass()
{

}

void USaveDatamanagerClass::Initialize()
{
	if (Instance)
	{
		Instance->RemoveFromRoot();
		Instance->MarkPendingKill();
	}

	Instance = this;
	Instance->AddToRoot();
}

void USaveDatamanagerClass::ShutDown()
{
	if (Instance)
	{
		Instance->RemoveFromRoot();
		Instance->MarkPendingKill();
		Instance = nullptr;
	}
}

void USaveDatamanagerClass::SavePlayerData()
{
	auto player = Cast<ABaseCharacterClass>(UGameplayStatics::GetPlayerCharacter(this, 0));
	FString _LevelPreFix = GetWorld()->StreamingLevelsPrefix;
	FString LevelName = player->GetWorld()->GetMapName();
	FString result = LevelName.Mid(_LevelPreFix.Len());

	FSaveData data;
	data.Player_Health = player->Health;
	data.Player_Balance = player->balance;
	data.Player_Location = player->GetActorLocation();
	data.Mission_Index = UGameMissionClass::GetInstance()->GetCurrentMissionIndex().CurrentMissionIndex;
	data.CurrentLevelName = result;

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(data, JsonString);
	check(FFileHelper::SaveStringToFile(*JsonString, *(FPaths::ProjectContentDir() + TEXT("SaveData.json"))));
}

void USaveDatamanagerClass::LoadPlayerData()
{
	FString JsonString;

	if (FFileHelper::LoadFileToString(JsonString, *(FPaths::ProjectContentDir() + TEXT("SaveData.json"))))
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			FSaveData data;
			data.Player_Health = JsonObject->GetNumberField(TEXT("Player_Health"));
			data.Player_Balance = JsonObject->GetNumberField(TEXT("Player_Balance"));
			data.Player_Location = ParseAsVector(JsonObject, TEXT("Player_Location"));
			data.Mission_Index = JsonObject->GetIntegerField(TEXT("Mission_Index"));
			data.CurrentLevelName = JsonObject->GetStringField(TEXT("CurrentLevelName"));

			PlayerData = data;
		}
	}
}

FVector USaveDatamanagerClass::ParseAsVector(TSharedPtr<FJsonObject> JasonValueObject, FString KeyName)
{
	FVector Loc;
	TSharedPtr<FJsonObject> PlayerLocationObject;
	PlayerLocationObject = JasonValueObject->GetObjectField(*KeyName);

	if(PlayerLocationObject.IsValid())
	{
		Loc.X = PlayerLocationObject->GetNumberField(TEXT("x"));
		Loc.Y = PlayerLocationObject->GetNumberField(TEXT("y"));
		Loc.Z = PlayerLocationObject->GetNumberField(TEXT("z"));
	}

	return  Loc;
}