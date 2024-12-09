// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

template <typename EnumType>
FORCEINLINE FString GetEnumName(const EnumType InValue)
{
	return StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(InValue));
}

template <typename EnumType>
FORCEINLINE FString GetEnumDisplayName(const EnumType InValue)
{
	return StaticEnum<EnumType>()->GetDisplayNameTextByValue(static_cast<int64>(InValue)).ToString();
}