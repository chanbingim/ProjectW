// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorBaseClass.generated.h"

UCLASS()
class PROJECTTNH_API AActorBaseClass : public AActor
{
	GENERATED_BODY()
private :
	

public:	
	// Sets default values for this actor's properties
	AActorBaseClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ActorActiveChange();

	UPROPERTY(EditAnywhere)
	bool bIsActive;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetActorActive(bool flag);

	UFUNCTION(BlueprintCallable)
	bool GetActorActive() { return bIsActive; }

	virtual void ActiveEevnetActor();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintCall_ActorActive();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintCall_ActorDisActive();
};
