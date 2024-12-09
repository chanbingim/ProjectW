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
	float ViewAngle = FMath::Acos(dot) * (180.0f / PI); // 라디안을 각도로 변환

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(_playerController->GetPawn());  // 플레이어 캐릭터를 무시

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

	//현재 월드에서 Ai Navigation이 활성화 되어있는지 확인
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_Actor->GetWorld());
	// Nav Mesh 데이터 강제 재구축
	if (nullptr == NavSys)
		return false;

	// NavData를 가져와서 NavMesh 상에서 유효한 경로를 계산
	FNavLocation _StargetLocation;
	FPathFindingQuery _Query;
	FPathFindingResult _Result;

	// Pathfinding 설정
	auto AiActor = Cast<APawn>(_Actor);
	//액터의 내비게이션 관련 속성 가져옴
	FNavAgentProperties NavAgentProps = AiActor->GetNavAgentPropertiesRef();

	_Query = FPathFindingQuery(_Actor, *NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate), _Actor->GetActorLocation(), TargetPoint);

	// 경로가 유효한지 확인
	_Result = NavSys->FindPathSync(NavAgentProps, _Query);

	return _Result.IsSuccessful(); // 성공적으로 경로를 찾았는지 반환
}

float UActorUtiliyCompoents::GetVectorAngle(FVector vec1, FVector vec2)
{
	auto Vec1_Normal = vec1.GetSafeNormal();
	auto Vec2_Normal = vec2.GetSafeNormal();

	//정규화를 시켰기 때문에 cos값을 구함
	float dot = FVector::DotProduct(Vec1_Normal, Vec2_Normal);

	//Cos값이 -1 에서 1사이이기 때문에 dot 값을 -1 ~ 1로 클램핑
	float clampDot = FMath::Clamp(dot, -1, 1);

	//cos에서 Acos곱하면 각도가 나옴
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