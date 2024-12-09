// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss_CharacterBase.h"
#include "ProjectTNH/DamageType/DT_LongSwordAttack.h"
#include "ProjectTNH/TNHGameInstance.h"
#include "ProjectTNH/UI/CustomWidget_PlayerStateBarBase.h"
#include "ProjectTNH/PlayerController/TNHPlayerController.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterController.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/AI/AI_DataTable/DT_AiETCDataRow.h"
#include "ProjectTNH/UI/HUD/TNHMainHUD.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "NiagaraFunctionLibrary.h"
#include "ProjectTNH/AI/TargetSelectComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Character/ALSPlayerCameraManager.h"
#include "MotionWarping.h"
#include "MotionWarpingComponent.h"
#include "ProjectTNH/Character/TNHCharacter.h"

void ABoss_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void ABoss_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoss_CharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

}

float ABoss_CharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float _Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return _Damage;
}

void ABoss_CharacterBase::Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance)
{
	CalculateKnockBackPosition();

	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ABaseCharacterClass::OnEnded_Victim);

	//GetMesh()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ABaseCharacterClass* Attacker = Cast<ABaseCharacterClass>(_ExecutionAttacker);

	//FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(_ExecutionAttacker->GetActorLocation(), GetActorLocation());
	FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _ExecutionAttacker->GetActorLocation());
	FVector NextLocation = _ExecutionAttacker->GetActorLocation();// + _ExecutionAttacker->GetActorForwardVector() * _BetweenDistance

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), GetActorLocation(), NextRotation, false, false, 0.2f, false,
		EMoveComponentAction::Move, LatentInfo);

	GetCharacterAnimInstance()->Montage_Play(_VictimMontage);

	OnExecutionStart();

	Health = FMath::Clamp(Health - ExecutionDamage, 0, MaxHealth);

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	if (mainHUD)
	{
		mainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);
		mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
	}

	AAi_CharacterController* AI_Controller = Cast<AAi_CharacterController>(GetController());

	if (AI_Controller)
	{
		AI_Controller->StopMovement();
	}

	GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, _VictimMontage);
}

void ABoss_CharacterBase::OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt)
{
	if (Health > 0)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(ExecutionStunTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(ExecutionStunTimer, FTimerDelegate::CreateLambda([&]()
				{
					if (!IsValid(this))  // 객체 유효성 검사
					{
						return;  // 객체가 유효하지 않으면 실행 중단
					}

					balance = Maxbalance;

					ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

					if (mainHUD)
					{
						mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
					}

					if (CurrentState == ECharacterState::Stun)
					{
						//GetMesh()->SetCollisionProfileName(FName("CharacterMesh"));
						GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
						GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

						GetCharacterAnimInstance()->Montage_JumpToSection(
							FName("End"),
							CharacterAnimationData->Stun_Reaction[0]);
						ClearDataAll();

						bIsRecoverdBalance = false;
					}
				}), ExecutionStunRecoveryTime, false);
		}

		OnExecutionEnd();
	}
	else
	{
		//BossPhase End

		CurrentState = ECharacterState::Dead;
		OnBossDead();
		OnDeaded.Broadcast();
	}
}

void ABoss_CharacterBase::OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage)
{
	//ParryCounter is not work On Boss
}

UAnimMontage* ABoss_CharacterBase::GroggyReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedMontage = ABaseCharacterClass::GroggyReaction(_DamageCauser, _Direction);

	if (balance > 0 || CurrentState == ECharacterState::Stun)
		return SelectedMontage;

	if (CurrentState == ECharacterState::Attack)
		IAttackEnd();

	CurrentState = ECharacterState::Stun;


	if (StateBar != nullptr)
	{
		ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
		//StateBar->SetVisibility(ESlateVisibility::Collapsed);

		auto DirectionWidget = GameMainHUD->FindCustomWidgetFromWidgetPool(this);
		if (DirectionWidget != nullptr)
			GameMainHUD->RemoveCustomWidgetFromWidgetPool(DirectionWidget);
	}

	return SelectedMontage;
}

UAnimMontage* ABoss_CharacterBase::DeathReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* Montage = Super::DeathReaction(_DamageCauser, _Direction);

	CurrentState = ECharacterState::Dead;
	OnBossDead();
	OnDeaded.Broadcast();

	return nullptr;
}

void ABoss_CharacterBase::CalculateKnockBackPosition()
{
	ATNHCharacter* Player = Cast<ATNHCharacter>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());

	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName("KnockBack"), GetActorLocation());

	if (Player)
	{
		FVector TargetLoc = Player->GetActorLocation();
		FRotator TargetRot = Player->GetActorRotation();

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(TargetLoc, GetActorLocation());

		LookAtRot.Pitch = 0.f;
		LookAtRot.Roll = 0.f;

		FVector TraceLocation = GetActorLocation() + (UKismetMathLibrary::GetForwardVector(LookAtRot) * (500.f + CharacterInfo->CharacterRadius));

		TEnumAsByte<ETraceTypeQuery> TraceType = UEngineTypes::ConvertToTraceType(ECC_WorldStatic);

		TArray<AActor*> Ignore;
		Ignore.AddUnique(this);
		Ignore.AddUnique(Player);

		FHitResult HitResult;

		FVector KnockBackLocation = GetActorLocation() + (UKismetMathLibrary::GetForwardVector(LookAtRot) * 500.f);

		bool bHit = UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), TraceLocation, TraceType,
			false, Ignore, EDrawDebugTrace::None, HitResult, true);

		if (bHit)
		{
			KnockBackLocation = HitResult.Location - (UKismetMathLibrary::GetForwardVector(LookAtRot) * CharacterInfo->CharacterRadius);
		}

		MotionWarping->AddOrUpdateWarpTargetFromLocation(FName("KnockBack"), KnockBackLocation);
	}
}
