// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFind_EnvQueryContext.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "../Character/BaseCharacterClass.h"

void UPlayerFind_EnvQueryContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AALSCharacter* PlayerPawn = Cast<ABaseCharacterClass>(QueryInstance.Owner.Get());

	if (PlayerPawn)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerPawn);
	}
}
 