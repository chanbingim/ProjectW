// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "TNHPlayerCameraBehavior.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UTNHPlayerCameraBehavior : public UALSPlayerCameraBehavior
{
	GENERATED_BODY()
	
public:
	void OnInit(class ATNHCharacter* _Owner);

private:
	class ATNHCharacter* TNHCharacter;

public:
	UPROPERTY(BlueprintReadOnly, Category = "CustomValue")
	class AActor* MainTarget;

	UPROPERTY(BlueprintReadOnly, Category = "CustomValue")
	bool ExistTarget;

	UPROPERTY(BlueprintReadOnly, Category = "CustomValue")
	int SpecifiedEnemyCount;

public:
	UFUNCTION(BlueprintCallable, Category = "CustomValue")
	bool IsRunning() { return (Gait == EALSGait::Running); }

	UFUNCTION(BlueprintCallable, Category = "CustomValue")
	bool IsSprinting() { return (Gait == EALSGait::Sprinting); }

	UFUNCTION(BlueprintCallable, Category = "CustomValue")
	bool IsMoving();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void NormalAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void CrushAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void ChargeStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void Hited();

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void GuardSuccessed();

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomValue")
	void Deaded();

	UFUNCTION()
	void TargetFinded(class AActor* _Target);

	UFUNCTION()
	void TargetLosted(AActor* _Target);
};
