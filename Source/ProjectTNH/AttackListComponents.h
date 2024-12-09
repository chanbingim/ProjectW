// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackListComponents.generated.h"

USTRUCT(BlueprintType)
struct FAttackCombo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> NextAttackname;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UAttackListComponents : public UActorComponent
{
	GENERATED_BODY()

private:
	FAttackCombo GetAttackComboData(FString _Dataname);
public:	
	// Sets default values for this component's properties
	UAttackListComponents();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackCombo> AttackList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> FristAttackList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> BreakAttackList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReactionTime;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UDataTable* AttackTable = nullptr;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> SelectComboList;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool MakeAttackComboList(int MaxComboCount);

	FString GetGuardBreakAttack();
	FString GetCombolistIndex(int Index);
	TArray<FString> GetSelectComboList() { return SelectComboList; }
	void ResetComboList();
};
