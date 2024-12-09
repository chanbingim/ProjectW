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

	// ���� ���� ��� (360���� NumberOfPoints�� ����)
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
		OverlapRadius,             // �ݰ�
		CheckObject,  // �˻��� ��ü Ÿ��
		ABaseCharacterClass::StaticClass(),  // ������ ���� Ŭ����
		IgnoreActors,   // �˻翡�� ������ ���͵�
		OverlapClosestActor            // ����� ���� ���͵�
	);

	// 3. �������� ���͸� ��ȸ�ϸ� ���ͺ� ���� ����� ���͸� ã��, ���ÿ� �и� ��Ģ�� �����մϴ�.
	for (AActor* Actor : OverlapClosestActor)
	{
		auto ownerPawn = Cast<ABaseCharacterClass>(OwnerActor);
		auto _overlapActor = Cast<ABaseCharacterClass>(Actor);

		if (!Actor || Actor == OwnerActor || _overlapActor->TeamNum != ownerPawn->TeamNum) continue;

		// �ڽŰ� ���� ������ �Ÿ��� ������ ����մϴ�.
		FVector ToActor = OwnerActor->GetActorLocation()- Actor->GetActorLocation();
		ToActor.Z = 0.0f;  // Z���� �����Ͽ� 2D ��鿡���� ���⸸ ���
		float Distance = ToActor.Size();  // �Ÿ� ���

		// �и� ��Ģ ����: �ʹ� ����� ���ʹ� �ݹ߷�(�и� ����)�� �����Ͽ� �־������� �մϴ�.
		if (Distance < DesiredSeparationDistance) // �Ÿ� < �ּ� �Ÿ�
		{
			// �Ÿ��� �ݺ�� ����� �и� ���� ���� ����
			float Strength = FMath::Clamp(1.0f / Distance, 0.0f, 1.0f);  // �Ÿ��� �������� ���� �ݹ߷�
			SeparationVector += ToActor * (Strength);  // �ݹ߷��� �� �и� ���Ϳ� �߰� (�ݴ� �������� �и�)
		}
	}

	if (!SeparationVector.IsNearlyZero())
	{
		auto OwnerCharacter = Cast<ABaseCharacterClass>(OwnerActor);

		if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
		{
			// �и� ���͸� �̵� �������� ��ȯ�Ͽ� ĳ���Ϳ� �̵� �Է� ����
			FVector SeparationDirection = SeparationVector.GetSafeNormal();  // ���⸸ ����
			float SeparationMagnitude = SeparationVector.Size();  // ���� ũ��

			// SeparationStrength�� ���� ������ ����
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

		//Ÿ�� ���� ������
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

