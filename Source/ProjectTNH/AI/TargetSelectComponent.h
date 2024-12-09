// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetSelectComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UTargetSelectComponent : public UActorComponent
{
	GENERATED_BODY()
private :
	UPROPERTY(VisibleAnyWhere)
	AActor* Owner;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> HostileEntities;

	UPROPERTY(EditAnywhere)
	int MaxHostileEntitiesCount;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UTargetSelectComponent();

	UFUNCTION()
	void SelectSingleTarget(AActor* _Target);

	UFUNCTION()
	void SetOwner(AActor* _Owner) { Owner = _Owner; }
	
	UFUNCTION()
	AActor* GetOwnerActor() { return Owner; }

	UFUNCTION()
	void ResetAllHostileData();

	UFUNCTION()
	void RemoveHostileData(AActor* _Target);

	UFUNCTION()
	void AddHostileData(AActor* _Target) { HostileEntities.Add(_Target); }
};
