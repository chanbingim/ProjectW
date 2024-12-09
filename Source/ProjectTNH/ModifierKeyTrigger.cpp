// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifierKeyTrigger.h"

#include "EnhancedPlayerInput.h"

ETriggerState UModifierKeyTrigger::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue ModifiedValue, float DeltaTime)
{
	if (IsActuated(ModifiedValue) && PlayerInput->IsPressed(ModifierKey))
	{
		return ETriggerState::Triggered;
	}

	return ETriggerState::None;
}

#if WITH_EDITOR
void UModifierKeyTrigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().IsEqual(GET_MEMBER_NAME_CHECKED(UModifierKeyTrigger, ModifierKey)))
	{
		if (!ModifierKey.IsModifierKey())
		{
			ModifierKey = EKeys::LeftControl;
		}
	}
}
#endif

