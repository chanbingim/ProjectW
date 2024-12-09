// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "ModifierKeyTrigger.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API UModifierKeyTrigger : public UInputTrigger
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Trigger Settings")
	FKey ModifierKey = EKeys::LeftControl;

protected:
	virtual ETriggerType GetTriggerType_Implementation() const override { return ETriggerType::Implicit; }
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
		FInputActionValue ModifiedValue, float DeltaTime) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
