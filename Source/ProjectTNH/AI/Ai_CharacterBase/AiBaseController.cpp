// Fill out your copyright notice in the Description page of Project Settings.


#include "AiBaseController.h"
#include "Ai_CharacterBase.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

AAiBaseController::AAiBaseController()
{
	ConstructorHelpers::FObjectFinder<UDataTable> PropertiesObj(TEXT("/Script/Engine.DataTable'/Game/04_DataTable/AI/DT_AIStatus.DT_AIStatus'"));
	if (PropertiesObj.Succeeded())
		AiSatusDataTable = PropertiesObj.Object;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("AISightConfig");
	AiPerceptionCom = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
}

void AAiBaseController::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAiBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnningPawn = Cast<AAi_CharacterBase>(InPawn);
}

void AAiBaseController::SetOwnningPawn(APawn* _Pawn)
{
	OwnningPawn = Cast<AAi_CharacterBase>(_Pawn);
	AttachToActor(_Pawn, FAttachmentTransformRules::KeepWorldTransform);
}

void AAiBaseController::DeadAiPawn(AActor* KillerPawn)
{
	UnPossess();
}

void AAiBaseController::SetPerceptionSystem()
{
	if (PropertiesData == nullptr)
		return;

	if (SightConfig && AiPerceptionCom)
	{
		SightConfig->SightRadius =  PropertiesData->SightRadius;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + AILoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = PropertiesData->SightAngle;
		SightConfig->SetMaxAge(0.1f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = AILastSeenLocation;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AiPerceptionCom->SetDominantSense(*SightConfig->GetSenseImplementation());
		AiPerceptionCom->ConfigureSense(*SightConfig);
	}
}

void AAiBaseController::SetAiStatusData(FString PawnType)
{
	if (this == nullptr || AiSatusDataTable == nullptr)
		return;

	PropertiesData = AiSatusDataTable->FindRow<FAIStatusRow>(FName(PawnType), "");
	OwnningPawn = Cast<AAi_CharacterBase>(GetPawn());

	SetPerceptionSystem();
}
