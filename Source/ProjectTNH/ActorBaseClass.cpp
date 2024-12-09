// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorBaseClass.h"

// Sets default values
AActorBaseClass::AActorBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActorBaseClass::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AActorBaseClass::ActorActiveChange()
{
	return bIsActive;
}

// Called every frame
void AActorBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorBaseClass::SetActorActive(bool flag)
{
	bIsActive = flag;

	ActorActiveChange();

	if(bIsActive)
		BlueprintCall_ActorActive();
	else
		BlueprintCall_ActorDisActive();
}

void AActorBaseClass::ActiveEevnetActor()
{

}

