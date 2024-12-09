// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TNHGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API ATNHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
private :
	UPROPERTY(VisibleAnywhere)
	class UTNHGameInstance* MainGameInstance;

	UPROPERTY(VisibleAnywhere)
	class UUAiCommander* AICommanderIns;

	UPROPERTY(EditAnywhere)
	float LockOnBattlefieldOffset;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;

public :
	ATNHGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UGameObjectPool* objectPool = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpdateAiRate;

	UFUNCTION()
	void AddActorsToPoolOnBeginPlay(AActor* _actor);

	UFUNCTION()
	void RemoveActorsToPool(AActor* _actor);

	UFUNCTION()
	float GetLockOnBattlefieldOffset() { return LockOnBattlefieldOffset; }
};
