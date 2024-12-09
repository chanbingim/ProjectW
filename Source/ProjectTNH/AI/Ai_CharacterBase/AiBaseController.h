// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectTNH/DataTable/DT_AIStatus.h"
#include "Perception/AIPerceptionTypes.h"
#include "AiBaseController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTNH_API AAiBaseController : public AAIController
{
	GENERATED_BODY()

private :
	

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	TArray<AActor*> PreviousAry;

	UPROPERTY(VisibleAnywhere)
	float CurrentSearchTime = 0.0f;

	UPROPERTY(VisibleAnywhere)
	class AAi_CharacterBase* OwnningPawn = nullptr;

	UPROPERTY(EditAnywhere)
	float AISightRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AILoseSightRadius = 50.f;

	UPROPERTY(EditAnywhere)
	float AIFieldOfView = 90.f;

	UPROPERTY(EditAnywhere)
	float AIForgetTime = 2.0f;

	UPROPERTY(EditAnywhere)
	float AILastSeenLocation = 900.f;

	UPROPERTY(EditAnywhere)
	float TargetSearchTime = 0.0f;

	UPROPERTY(EditAnywhere)
	class UAIPerceptionComponent* AiPerceptionCom = nullptr;

	UPROPERTY(EditAnywhere)
	class UAISenseConfig_Sight* SightConfig = nullptr;

	UPROPERTY(EditAnywhere, Category = "AISatusData")
	class UDataTable* AiSatusDataTable = nullptr;

public :
	AAiBaseController();

	UFUNCTION()
	void SetOwnningPawn(APawn* _Pawn);

	virtual void DeadAiPawn(AActor* KillerPawn);

	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void SetAiStatusData(FString PawnType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float SafetyDistance = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	TArray<AActor*> TAry_TargetEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	TArray<AActor*> TAry_Team;

	UFUNCTION()
	float GetAiSightRadius() { return AISightRadius; }

	FAIStatusRow* PropertiesData = nullptr;
};
