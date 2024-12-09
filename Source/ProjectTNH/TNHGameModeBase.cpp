// Fill out your copyright notice in the Description page of Project Settings.

#include "TNHGameModeBase.h"
#include "TNHGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/TNHMainHUD.h"
#include "AI/UAiCommander.h"
#include "GameObjectPool.h"

ATNHGameModeBase::ATNHGameModeBase()
{
	objectPool = CreateDefaultSubobject<UGameObjectPool>(TEXT("objectPool"));
	objectPool->PoolSize = 5;

	objectPool->SetWorld();
}

void ATNHGameModeBase::AddActorsToPoolOnBeginPlay(AActor* _actor)
{
	objectPool->AddActorInstance(_actor);
}

void ATNHGameModeBase::RemoveActorsToPool(AActor* _actor)
{
	objectPool->RemoveActorInstance(_actor);
}

void ATNHGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	

}

void ATNHGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	MainGameInstance = Cast<UTNHGameInstance>(GetGameInstance());
	AICommanderIns = GetWorld()->GetSubsystem<UUAiCommander>();
	AICommanderIns->CustonInitialize();

	auto playerController = UGameplayStatics::GetPlayerController(this, 0);
	playerController->SetInputMode(FInputModeGameOnly());
	playerController->bShowMouseCursor = false;
}