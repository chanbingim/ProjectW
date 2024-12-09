// Fill out your copyright notice in the Description page of Project Settings.


#include "GameObjectPool.h"
#include "AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void UGameObjectPool::BeginPlay()
{
	Super::BeginPlay();

	if (OwnningWorld == nullptr)
	{
		OwnningWorld = GetWorld();
	}

}

UObject* UGameObjectPool::CreateInstance_Blueprint(TSubclassOf<UObject> ActorClass)
{
	return GetOrCreateInstance(ActorClass);
}

template<typename UObjectT>
UObjectT* UGameObjectPool::AddActiveActorInternal(TSubclassOf<UObjectT> ActorClass)
{
	if (!ActorClass)
	{
		return nullptr;
	}

	UObjectT* ActorInstance = nullptr;
	if (InactiveActors.Num() > 0)
	{
		auto CopyAry = InactiveActors;

		for (UObjectT* InactiveActor : CopyAry)
		{
			if (InactiveActor == nullptr)
				continue;

			if (InactiveActor->GetClass() == ActorClass)
			{
				ActorInstance = InactiveActor;
				break;
			}
		}
		if(ActorInstance)
			InactiveActors.Remove(ActorInstance);
	}

	if (!ActorInstance)
	{
		ActorInstance = NewObject<UObjectT>(GetWorld(), ActorClass);
	}

	if (ActorInstance)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorInstance = GetWorld()->SpawnActor<AAi_CharacterBase>(ActorInstance->GetClass(), SpawnParams);
		ActiveActors.Add(ActorInstance);
	}

	return ActorInstance;
}

void UGameObjectPool::SpawnActiveActors(FVector SpawnLoc, EPawnType Type)
{
	for (UObject* InactiveActor : ActiveActors)
	{
		auto spawnPawn = Cast<AAi_CharacterBase>(InactiveActor);
		spawnPawn->pawnType = Type;
		spawnPawn->TeamNum = 1;

		GetWorld()->SpawnActor<AAi_CharacterBase>(spawnPawn->GetClass(), SpawnLoc, FRotator::ZeroRotator);
	}
}

void UGameObjectPool::SetWorld()
{
	if (OwnningWorld == nullptr)
	{
		OwnningWorld = GetWorld();
	}
}
