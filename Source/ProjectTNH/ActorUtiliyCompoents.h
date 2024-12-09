// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorUtiliyCompoents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UActorUtiliyCompoents : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorUtiliyCompoents();

protected:

public:	
	UFUNCTION(BlueprintCallable)
	static bool IsActorInPlayerView(AActor* _actor);

	UFUNCTION(BlueprintCallable)
	static bool IsAIPathVaild(AActor* _Actor, FVector TargetPoint);
	
	UFUNCTION(BlueprintCallable)
	static float GetVectorAngle(FVector vec1, FVector vec2);

	UFUNCTION(BlueprintCallable)
	static int GetActorHitDirection4Way(float Degree);

	UFUNCTION(BlueprintCallable)
	static int GetActorHitDirection8Way(float Degree);

	template <typename UObjectT = UObject>
	static TArray<UObjectT*> ArrayASCSort(TArray<UObjectT *> _List,FVector Location);

	template <typename UObjectT = UObject>
	static TArray<UObjectT*> ArrayDECSort(TArray<UObjectT *> _List, FVector Location);
};


template<typename UObjectT>
TArray<UObjectT*> UActorUtiliyCompoents::ArrayASCSort(TArray<UObjectT*> _List, FVector Location)
{
	_List.Sort([Location](const AActor& _TargetA, const AActor& _TargetB)
		{
			float DistanceA = FVector::DistSquared(Location, _TargetA.GetActorLocation());
			float DistanceB = FVector::DistSquared(Location, _TargetB.GetActorLocation());

			return DistanceA < DistanceB;  // 가까운 순으로 정렬
		});

	return _List;
}

template<typename UObjectT>
TArray<UObjectT*> UActorUtiliyCompoents::ArrayDECSort(TArray<UObjectT*> _List, FVector Location)
{
	_List.Sort([Location](const AActor& _TargetA, const AActor& _TargetB)
		{
			float DistanceA = FVector::DistSquared(Location, _TargetA.GetActorLocation());
			float DistanceB = FVector::DistSquared(Location, _TargetB.GetActorLocation());

			return DistanceA > DistanceB;  // 가까운 순으로 정렬
		});

	return _List;
}