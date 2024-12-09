// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackListComponents.h"
#include "Kismet/GameplayStatics.h"
#include "TNHGameInstance.h"

// Sets default values for this component's properties
UAttackListComponents::UAttackListComponents()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	// ...
}

// Called when the game starts
void UAttackListComponents::BeginPlay()
{
	Super::BeginPlay();

	auto TNHGameIns = Cast<UTNHGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (TNHGameIns != nullptr)
		AttackTable = TNHGameIns->AttackDataTable;
}

// Called every frame
void UAttackListComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAttackListComponents::MakeAttackComboList(int MaxComboCount)
{
	if (AttackList.Num() <= 0)
		return false;
	else
	{
		int randInteger = FMath::RandRange(0, FristAttackList.Num() - 1);
		FAttackCombo fristname = GetAttackComboData(FristAttackList[randInteger]);
		SelectComboList.Add(fristname.AttackName);
		auto TempComboCount = MaxComboCount - 1;

		while (TempComboCount > 0)
		{
			if (fristname.NextAttackname.Num() <= 0 || fristname.AttackName == "")
			{
				if (SelectComboList.Num() > 0)
					break;
				else
					return false;
			}
			else
			{
				auto random = FMath::RandRange(0, fristname.NextAttackname.Num() - 1);
				auto NextAttack = fristname.NextAttackname[random];

				SelectComboList.Add(NextAttack);
				fristname = GetAttackComboData(NextAttack);
				TempComboCount -= 1;
			}
		}
	}
	return true;
}

FAttackCombo UAttackListComponents::GetAttackComboData(FString _Dataname)
{
	FAttackCombo TempData;
	TempData.AttackName = "";

	for (auto Data : AttackList)
	{
		if (Data.AttackName == _Dataname)
		{
			return Data;
		}
	}

	return TempData;
}

FString UAttackListComponents::GetGuardBreakAttack()
{
	if (BreakAttackList.Num() <= 0)
		return "";

	int selectIndex =FMath::RandRange(0, BreakAttackList.Num() - 1);
	return BreakAttackList[selectIndex];
}

FString UAttackListComponents::GetCombolistIndex(int Index)
{
	if(SelectComboList.Num() - 1 >= Index)
		return SelectComboList[Index];

	return "";
}

void UAttackListComponents::ResetComboList()
{
	SelectComboList.Empty();
}
