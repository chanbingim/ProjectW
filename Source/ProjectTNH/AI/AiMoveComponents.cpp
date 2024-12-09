// Fill out your copyright notice in the Description page of Project Settings.


#include "AiMoveComponents.h"
#include "Ai_CharacterBase/AiBaseController.h"
#include "Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/BoundBoxBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Ai_CharacterBase/AiBaseController.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"
#include "UAiCommander.h"
#include "../Character/BaseCharacterClass.h"

// Sets default values for this component's properties
UAiMoveComponents::UAiMoveComponents()
{

}

// Called when the game starts
void UAiMoveComponents::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().ClearTimer(MoveDelayHandle);
}

void UAiMoveComponents::InitializeComponent()
{
	Super::InitializeComponent();

	GetWorld()->GetTimerManager().ClearTimer(MoveDelayHandle);
}

void UAiMoveComponents::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UAiMoveComponents::InitializeMoveComponent()
{

}

void UAiMoveComponents::SearchAiMoveArea()
{
	AryMovePos.Empty();

	if (OwnerActor == nullptr)
		return;
	auto _Owner = Cast<ABaseCharacterClass>(OwnerActor);

	if (_Owner->TargetActor == nullptr)
		return;

	float betDis = OwnerActor->GetDistanceTo(_Owner->TargetActor);
	FVector CenterPos = OwnerActor->GetActorLocation();
	FVector ForwardVector = OwnerActor->GetActorForwardVector();
	bool isWithinSafetyRange = betDis <= _SafetyRange;

	// 각도 간격 계산 (360도를 NumberOfPoints로 나눔)
	float AngleStepRad = FMath::DegreesToRadians(360.0f / NumberOfPointArea);
	for (int i = 0; i <= NumberOfPointArea; ++i)
	{
		float CurrentAngle = AngleStepRad * i;

		float CosAngle = FMath::Cos(CurrentAngle);
		float SinAngle = FMath::Sin(CurrentAngle);

		FVector PointLocation = CenterPos + FVector(MinionDectectionRadius * CosAngle, MinionDectectionRadius * SinAngle, 0.0f);
		FVector PointDir = (PointLocation - CenterPos).GetSafeNormal();

		float dot = FVector::DotProduct(ForwardVector, PointDir);
		float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

		if ((isWithinSafetyRange && angle >= 90.f) || (!isWithinSafetyRange && angle < 90.f))
		{
			AryMovePos.Add(PointLocation);
		}
	}
}

void UAiMoveComponents::MovementAccordingToStatus()
{
	float RandomDelay = FMath::RandRange(0.1f, MoveDelayTime);

	if (!GetWorld()->GetTimerManager().IsTimerActive(MoveDelayHandle))
	{
		TWeakObjectPtr<UAiMoveComponents> WeakThis = this;

		GetWorld()->GetTimerManager().SetTimer(MoveDelayHandle, FTimerDelegate::CreateLambda([WeakThis]()
			{
				if (WeakThis.IsValid())
				{
					auto TargetPtr = WeakThis.Get();

					bool flag = false;
					ABaseCharacterClass* Owner = Cast<ABaseCharacterClass>(TargetPtr->GetOwnerActor());

					if (Owner == nullptr)
						return;

					switch (Owner->CurrentState)
					{
					case ECharacterState::Move:
						if (Owner->pawnType < EPawnType::NormalArcher)
						{
							TargetPtr->SearchAiMoveArea();
							flag = TargetPtr->MoveToTargetPoint(Owner->GetActorLocation());
						}
						break;
					case ECharacterState::Run:
						Owner->SetCharacterWalkSpeed(Owner->runSpeed);
						if (Owner->MissionObject != nullptr)
						{
							auto MissionPoint = Cast<ABoundBoxBase>(Owner->MissionObject)->GetRandomPointInBox();
							flag = TargetPtr->MoveToTargetPoint(MissionPoint);
						}
						else
							flag = false;
						break;
					}
					if (!flag)
						Owner->CurrentState = ECharacterState::Idle;

					TargetPtr->GetWorld()->GetTimerManager().ClearTimer(TargetPtr->MoveDelayHandle);
				}
			}), RandomDelay, false);
	}
}

void UAiMoveComponents::ClearComponent()
{
	GetWorld()->GetTimerManager().ClearTimer(MoveDelayHandle);
}

void UAiMoveComponents::GetShereOverlapActors()
{
	if (OwnerActor == nullptr)
		return;

	int NumSectors = 5;
	TArray<float> MinDistances;
	TArray<AActor*> ClosestActorsPerSector;
	MinDistances.Init(0, NumSectors);
	ClosestActorsPerSector.Init(nullptr, NumSectors);
	FVector SeparationVector = FVector::ZeroVector;

	if (!CheckObject.Contains(EObjectTypeQuery::ObjectTypeQuery1))
		CheckObject.Add(EObjectTypeQuery::ObjectTypeQuery1);

	if(!IgnoreActors.Contains(OwnerActor))
		IgnoreActors.Add(OwnerActor);

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		OwnerActor->GetActorLocation(),
		OverlapRadius,             // 반경
		CheckObject,  // 검사할 객체 타입
		ABaseCharacterClass::StaticClass(),  // 가져올 액터 클래스
		IgnoreActors,   // 검사에서 제외할 액터들
		OverlapClosestActor            // 결과로 얻은 액터들
	);

	// 3. 오버랩된 액터를 순회하며 섹터별 가장 가까운 액터를 찾고, 동시에 분리 규칙을 적용합니다.
	for (AActor* Actor : OverlapClosestActor)
	{
		auto ownerPawn = Cast<ABaseCharacterClass>(OwnerActor);
		auto _overlapActor = Cast<ABaseCharacterClass>(Actor);

		if (!Actor || Actor == OwnerActor || _overlapActor->TeamNum != ownerPawn->TeamNum) continue;

		// 자신과 액터 사이의 거리와 방향을 계산합니다.
		FVector ToActor = OwnerActor->GetActorLocation()- Actor->GetActorLocation();
		ToActor.Z = 0.0f;  // Z축은 무시하여 2D 평면에서의 방향만 계산
		float Distance = ToActor.Size();  // 거리 계산

		// 분리 규칙 적용: 너무 가까운 액터는 반발력(분리 벡터)을 적용하여 멀어지도록 합니다.
		if (Distance < DesiredSeparationDistance) // 거리 < 최소 거리
		{
			// 거리의 반비례 관계로 분리 벡터 강도 조절
			float Strength = FMath::Clamp(1.0f / Distance, 0.0f, 1.0f);  // 거리가 가까울수록 강한 반발력
			SeparationVector += ToActor * (Strength);  // 반발력을 총 분리 벡터에 추가 (반대 방향으로 밀림)
		}
	}

	if (!SeparationVector.IsNearlyZero())
	{
		auto OwnerCharacter = Cast<ABaseCharacterClass>(OwnerActor);

		if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
		{
			// 분리 벡터를 이동 방향으로 변환하여 캐릭터에 이동 입력 적용
			FVector SeparationDirection = SeparationVector.GetSafeNormal();  // 방향만 추출
			float SeparationMagnitude = SeparationVector.Size();  // 힘의 크기

			// SeparationStrength로 힘의 강도를 조절
			OwnerCharacter->AddMovementInput(SeparationDirection, SeparationMagnitude * SeparationStrength);
		}
	}
}

bool UAiMoveComponents::MoveToTargetPoint(FVector _TargetPoint)
{
	float TargetDistance;
	bool MoveAblePath = false;
	auto AiController = Cast<AAIController>(OwnerActor->GetInstigatorController());

	if (nullptr == AiController) return false;
	auto TargetAi = AiController->GetFocusActor();

	if (TargetAi != nullptr)
	{
		TargetDistance = OwnerActor->GetDistanceTo(TargetAi);

		int RandomIndex = FMath::RandRange(0, AryMovePos.Num() - 1);
		if (AryMovePos.Num() <= RandomIndex)
			return false;

		//타겟 지정 포지션
		FVector TargetPoint = AryMovePos[RandomIndex];
		AiController->MoveToLocation(TargetPoint);
		return true;
	}
	else
	{
		if (_TargetPoint != OwnerActor->GetActorLocation())
		{

			AiController->SetFocalPoint(_TargetPoint);
			AiController->MoveToLocation(_TargetPoint);
			return true;
		}
	}
	return false;
}

