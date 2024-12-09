// Fill out your copyright notice in the Description page of Project Settings.


#include "TNHGameInstance.h"
#include "UI/HUD/TNHMainHUD.h"
#include "SaveDatamanagerClass.h"
#include "GameMissionComponets.h"

void UTNHGameInstance::Init()
{
	Super::Init();

    if (!SaveManager)
    {
        SaveManager = NewObject<USaveDatamanagerClass>(this, subClassSaveManager);
        SaveManager->Initialize();
    }

    if (!MissionManager)
    {
        MissionManager = NewObject<UGameMissionClass>(this, subClassMissionManager);
        MissionManager->Initialize();
    }
}

void UTNHGameInstance::Shutdown()
{
    Super::Shutdown();

    SaveManager->ShutDown();
    MissionManager->ShutDown();
}

void UTNHGameInstance::InitializeHUD(AHUD* HUD)
{
	auto TNHHud = Cast < ATNHMainHUD>(HUD);

	TNHHud->Init();
}
