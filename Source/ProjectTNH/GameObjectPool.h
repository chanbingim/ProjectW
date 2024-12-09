// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include  "Components/ActorComponent.h"
#include "GameObjectPool.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTTNH_API UGameObjectPool : public UActorComponent
{
	GENERATED_BODY()
public:	
	UGameObjectPool() = default;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> BaseCalss = nullptr;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* SpawnAiBaseTree = nullptr;

	const TArray<UObject*> GetActiveActors() const { return ActiveActors; }

	UFUNCTION(BlueprintCallable)
	UObject* CreateInstance_Blueprint(TSubclassOf<UObject> ActorClass);

	template <typename UObjectT = UObject>
	UObjectT* GetOrCreateInstance(TSubclassOf<UObjectT> WidgetClass)
	{
		return AddActiveActorInternal(WidgetClass);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
	int PoolSize;

	template <typename UObjectT = UObject>
	void RemoveActorInstance(UObjectT* Item)
	{
		for (UObject* InactiveActor : ActiveActors)
		{
			if (InactiveActor == Item)
			{
				ActiveActors.Remove(InactiveActor);
				InactiveActors.Add(InactiveActor);
				break;
			}
		}
	}

	template <typename UObjectT = UObject>
	void AddActorInstance(UObjectT* ActorClass)
	{
		if (!ActiveActors.Contains(ActorClass))
			ActiveActors.Add(ActorClass);
	}


	UFUNCTION(BlueprintCallable)
	void SpawnActiveActors(FVector SpawnLoc, EPawnType Type);

	void SetWorld();
protected:
	virtual void BeginPlay() override;

private:
	template <typename UObjectT = UObject>
	UObjectT* AddActiveActorInternal(TSubclassOf<UObjectT> WidgetClass);

	UPROPERTY(VisibleAnywhere,Transient, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UObject>> ActiveActors;

	UPROPERTY(VisibleAnywhere, Transient, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UObject>> InactiveActors;

	TWeakObjectPtr<UWorld> OwnningWorld;
};


