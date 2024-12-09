// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorUtiliyCompoents.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UActorUtiliyCompoents::UActorUtiliyCompoents()
{

	// ...
}

bool UActorUtiliyCompoents::IsActorInPlayerView(AActor* _actor)
{
	auto _playerController = UGameplayStatics::GetPlayerController(_actor, 0);

	FVector CameraLocation;
	FRotator CameraRotation;
	_playerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraForwardVector = CameraRotation.Vector();
	FVector ToActorDir = (_actor->GetActorLocation() - CameraLocation).GetSafeNormal();

	float playerFov = _playerController->PlayerCameraManager->GetFOVAngle();
	float dot = FVector::DotProduct(CameraForwardVector, ToActorDir);
	float ViewAngle = FMath::Acos(dot) * (180.0f / PI); // ������ ������ ��ȯ

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(_playerController->GetPawn());  // �÷��̾� ĳ���͸� ����

	if(ViewAngle >= (playerFov / 2.0f))
	{
		float dis = _playerController->GetPawn()->GetDistanceTo(_actor);

		if (dis >= 900)
			return false;
	}

	return  true;
}

bool UActorUtiliyCompoents::IsAIPathVaild(AActor* _Actor, FVector TargetPoint)
{
	if (nullptr == _Actor)
		return false;

	//���� ���忡�� Ai Navigation�� Ȱ��ȭ �Ǿ��ִ��� Ȯ��
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_Actor->GetWorld());
	// Nav Mesh ������ ���� �籸��
	if (nullptr == NavSys)
		return false;

	// NavData�� �����ͼ� NavMesh �󿡼� ��ȿ�� ��θ� ���
	FNavLocation _StargetLocation;
	FPathFindingQuery _Query;
	FPathFindingResult _Result;

	// Pathfinding ����
	auto AiActor = Cast<APawn>(_Actor);
	//������ ������̼� ���� �Ӽ� ������
	FNavAgentProperties NavAgentProps = AiActor->GetNavAgentPropertiesRef();

	_Query = FPathFindingQuery(_Actor, *NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate), _Actor->GetActorLocation(), TargetPoint);

	// ��ΰ� ��ȿ���� Ȯ��
	_Result = NavSys->FindPathSync(NavAgentProps, _Query);

	return _Result.IsSuccessful(); // ���������� ��θ� ã�Ҵ��� ��ȯ
}

float UActorUtiliyCompoents::GetVectorAngle(FVector vec1, FVector vec2)
{
	auto Vec1_Normal = vec1.GetSafeNormal();
	auto Vec2_Normal = vec2.GetSafeNormal();

	//����ȭ�� ���ױ� ������ cos���� ����
	float dot = FVector::DotProduct(Vec1_Normal, Vec2_Normal);

	//Cos���� -1 ���� 1�����̱� ������ dot ���� -1 ~ 1�� Ŭ����
	float clampDot = FMath::Clamp(dot, -1, 1);

	//cos���� Acos���ϸ� ������ ����
	float AngleRad = FMath::Acos(clampDot);
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	return AngleDeg;
}

int UActorUtiliyCompoents::GetActorHitDirection4Way(float Degree)
{
	if (Degree >= 45 && Degree < 135) // Front
	{
		return 0;
	}
	else if (Degree >= 135 && Degree < 225) // Left
	{
		return 1;
	}
	else if (Degree >= 225 && Degree < 315) // Rear
	{
		return 2;
	}
	else // Right
	{
		return 3;
	}

	return -1;
}


int UActorUtiliyCompoents::GetActorHitDirection8Way(float Degree)
{
	if (Degree >= 22.5f && Degree < 67.5f) // Front Right
	{
		return 0; // Front Right
	}
	else if (Degree >= 67.5f && Degree < 112.5f) // Front
	{
		return 1; // Front
	}
	else if (Degree >= 112.5f && Degree < 157.5f) // Front Left
	{
		return 2; // Front Left
	}
	else if (Degree >= 157.5f && Degree < 202.5f) // Left
	{
		return 3; // Left
	}
	else if (Degree >= 202.5f && Degree < 247.5f) // Back Left
	{
		return 4; // Back Left
	}
	else if (Degree >= 247.5f && Degree < 292.5f) // Back
	{
		return 5; // Back
	}
	else if (Degree >= 292.5f && Degree < 337.5f) // Back Right
	{
		return 6; // Back Right
	}
	else // Right
	{
		return 7; // Right
	}

	return -1;
}