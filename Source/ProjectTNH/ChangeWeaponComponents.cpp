// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeWeaponComponents.h"
#include "Components/SphereComponent.h"
#include "Character/BaseCharacterClass.h"
#include "AI/Ai_CharacterBase/AiBaseController.h"
#include "AI/Ai_CharacterBase/Ai_CharacterBase.h"

// Sets default values for this component's properties
UChangeWeaponComponents::UChangeWeaponComponents()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	ChangeRange = CreateDefaultSubobject<USphereComponent>("OverlapRange");
	if (ChangeRange->OnComponentBeginOverlap.Contains(this, FName(TEXT("BeginOverlapEvent"))) == false)
	{
		ChangeRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOverlapEvent);
	}
	if (ChangeRange->OnComponentEndOverlap.Contains(this, FName(TEXT("EndOverlapEvent"))) == false)
	{
		ChangeRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::EndOverlapEvent);
	}
}

void UChangeWeaponComponents::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwnerClass == nullptr)
		return;
	 
	auto OwnerPawn = Cast<AAi_CharacterBase>(OwnerClass);
	if (nullptr != ChangeWeaponClass)
	{
		FTransform SpawnTransform = OwnerPawn->GetMesh()
			->GetSocketTransform(FName(SocketNameSubWeapon), ERelativeTransformSpace::RTS_Component);
		SubWeapon = GetWorld()->SpawnActor<AActor>(ChangeWeaponClass, SpawnTransform);

		if (SubWeapon == nullptr)
			return;

		SubWeapon->AttachToComponent(OwnerPawn->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale, FName(SocketNameSubWeapon));
	}
}

void UChangeWeaponComponents::BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OwnerPawn = Cast<AAi_CharacterBase>(OwnerClass);
	if (OwnerPawn == nullptr)
		return;

	if (Cast<APawn>(OtherActor) == nullptr && OwnerPawn->CurrentState != ECharacterState::Idle)
		return;

	float delayTime = FMath::RandRange(0.1, 1.0);
	OwnerPawn->CurrentState = ECharacterState::ChangeWeapon;

	if (!GetWorld()->GetTimerManager().IsTimerActive(ChangeWeaponTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(ChangeWeaponTimer, FTimerDelegate::CreateLambda([&]()
			{
				if (!IsValid(this))  // 객체 유효성 검사
				{
					return;  // 객체가 유효하지 않으면 실행 중단
				}

				auto OwnerPawn = Cast<ABaseCharacterClass>(OwnerClass);
				
				if (nullptr != MainWeaponToSubWeaponAnim)
					OwnerPawn->GetCharacterAnimInstance()->Montage_Play(MainWeaponToSubWeaponAnim);

				GetWorld()->GetTimerManager().ClearTimer(ChangeWeaponTimer);
			}), delayTime, false);
	}
}

void UChangeWeaponComponents::EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OwnerPawn = Cast<AAi_CharacterBase>(OwnerClass);
	if (OwnerPawn == nullptr)
		return;

	if (Cast<APawn>(OtherActor) == nullptr && OwnerPawn->CurrentState != ECharacterState::Idle)
		return;

	float delayTime = FMath::RandRange(0.1, 1.0);
	OwnerPawn->CurrentState = ECharacterState::ChangeWeapon;

	if (!GetWorld()->GetTimerManager().IsTimerActive(ChangeWeaponTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(ChangeWeaponTimer, FTimerDelegate::CreateLambda([&]()
			{
				if (!IsValid(this))  // 객체 유효성 검사
				{
					return;  // 객체가 유효하지 않으면 실행 중단
				}

				auto OwnerPawn = Cast<ABaseCharacterClass>(OwnerClass);
				
				if (nullptr != SubWeaponToMainWeaponAnim)
					OwnerPawn->GetCharacterAnimInstance()->Montage_Play(SubWeaponToMainWeaponAnim);

				GetWorld()->GetTimerManager().ClearTimer(ChangeWeaponTimer);
			}), delayTime, false);
	}
}

void UChangeWeaponComponents::AttachWeapon(AActor* _Item, FString ToSocket, EPawnType ChangeType, bool bIsSettingData)
{
	auto OwnerPawn = Cast<ABaseCharacterClass>(OwnerClass);
	auto OwnerContorller = Cast<AAiBaseController>(OwnerPawn->GetController());

	if (nullptr != _Item && ToSocket != "")
	{
		_Item->AttachToComponent(OwnerPawn->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale, FName(SocketNameSubWeapon));
	}
	OwnerPawn->pawnType = ChangeType;

	if (bIsSettingData)
	{
		OwnerPawn->SettingPawnDataTable();
		OwnerContorller->SetAiStatusData(OwnerPawn->GetPawnType());
		OwnerPawn->CurrentState = ECharacterState::Idle;
	}
		
}


