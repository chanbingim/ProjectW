// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBaseClass.h"
#include "BoundBoxBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ABoundBoxBase : public AActorBaseClass
{
	GENERATED_BODY()
public :
	ABoundBoxBase();

protected :
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponents;
public :
	UFUNCTION(BlueprintCallable)
	FVector GetRandomPointInBox();
};
