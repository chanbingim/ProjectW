// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterClass.h"
#include "ProjectTNH/DamageType/DT_LongSwordAttack.h"
#include "../TNHGameInstance.h"
#include "../UI/CustomWidget_PlayerStateBarBase.h"
#include "../PlayerController/TNHPlayerController.h"
#include "../AI/Ai_CharacterBase/Ai_CharacterController.h"
#include "../AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "../UI/HUD/TNHMainHUD.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "ProjectTNH/AI/AI_DataTable/DT_AiETCDataRow.h"
#include "NiagaraFunctionLibrary.h"
#include "../AI/TargetSelectComponent.h"
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
#include "ProjectTNH/ProjectTNH.h"

// Sets default values
ABaseCharacterClass::ABaseCharacterClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (nullptr == HitComponents)
	{
		HitComponents = CreateDefaultSubobject<UDidItHitActorComponent>(TEXT("HitComponents"));
	}

	if (nullptr == Stimulisoure)
	{
		Stimulisoure = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulisoure"));
	}
	if (nullptr == MotionWarping)
	{
		MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	}
	if (nullptr == LockOnPoint)
	{
		LockOnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LockOnPoint"));
		LockOnPoint->SetupAttachment(RootComponent);
	}

	if (nullptr == _TargetSelectComponents)
	{
		_TargetSelectComponents = CreateDefaultSubobject<UTargetSelectComponent>(TEXT("TargetSelectComponents"));
	}
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshFinder(TEXT("'/Engine/BasicShapes/Plane.Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> CircleRingFinder(TEXT("'/Game/LJH/MI_CircleRing.MI_CircleRing'"));

	if (OnDeaded.Contains(this, FName(TEXT("Blueprint_DeathCall"))) == false)
	{
		OnDeaded.AddDynamic(this, &ThisClass::Blueprint_DeathCall);
	}
}

// Called when the game starts or when spawned
void ABaseCharacterClass::BeginPlay()
{
	Super::BeginPlay();

	InitData();
}

bool ABaseCharacterClass::CheckGuardableDirection(ECombatDirection _Direction, EReactionType _ReactionType)
{
	return(_Direction != CurrentDirection && _ReactionType != EReactionType::ERT_GuardBreak);
}

bool ABaseCharacterClass::CheckGuardSuccess(AActor* Enemy)
{
	FVector Direction = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector Forward = GetActorForwardVector().GetSafeNormal();

	float BetweenAngle = FMath::Acos(FVector::DotProduct(Forward, Direction));

	float BetweenDegree = FMath::RadiansToDegrees(BetweenAngle);

	return (BetweenDegree <= GuardDegree);
}

void ABaseCharacterClass::OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage)
{
	FVector HitActorLocation = GetActorLocation();
	FRotator HitActorRotation = GetActorRotation();
	FVector HitCauserLocation = _ParryCauser->GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HitActorLocation, HitCauserLocation);

	int Direction = FMath::RoundToInt((LookAtRotation.Yaw - HitActorRotation.Yaw - 90.f)) * -1;

	UAnimMontage* HitReaction = HighDamageReaction(_ParryCauser, Direction);
	ApplyDamageBalance(_BalanceDamage);

	if (HitReaction)
	{
		FOnMontageEnded OnEnded;

		LastestApplyDamagePawn = Cast<ABaseCharacterClass>(_ParryCauser);

		OnEnded.BindUObject(this, &ABaseCharacterClass::OnEnded_ParryHitReaction);

		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, HitReaction);
	}
}

// Called every frame
void ABaseCharacterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StateBar != nullptr || CurrentState != ECharacterState::Execution)
	{
		if (RecoveredTime > 0)
			RecoveredTime -= DeltaTime;
		else
			RecoverdBalace();
	}
}

void ABaseCharacterClass::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurrentKnockBackAnim)
	{
		if (GetCharacterAnimInstance()->Montage_IsPlaying(CurrentKnockBackAnim))
		{
			GetCharacterAnimInstance()->Montage_JumpToSection(FName("End"), CurrentKnockBackAnim);
		}
		FTimerHandle GroundTimer;
		float GroundTime = 2.f;

		GetWorld()->GetTimerManager().SetTimer(GroundTimer, FTimerDelegate::CreateUObject(this, &ThisClass::KnockBackCheckRagdoll), GroundTime, false);

		CurrentKnockBackAnim = nullptr;
	}
}

float ABaseCharacterClass::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (EventInstigator == nullptr || CurrentState >= ECharacterState::Dead)
		return 0.0f;

	ABaseCharacterClass* ApplyDamagePawn = Cast<ABaseCharacterClass>(EventInstigator->GetPawn());
	if(!ApplyDamagePawn)
		return 0.0f;

	LastestApplyDamagePawn = ApplyDamagePawn;

	LastBeAttackedCharacter = ApplyDamagePawn;
	if (ApplyDamagePawn->TeamNum == TeamNum ||
		(CurrentState == ECharacterState::Dodge && ApplyDamagePawn->AttackDirection == ECombatDirection::Crash))
		return 0.0f;

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	float TakeDamageAmount = Damage;
	float TakeBalanceAmount = ApplyDamagePawn->BalanceAmount;
	float TakeBlockBalanceAmount = ApplyDamagePawn->BlockBalanceAmount;
	float TakeParryBalanceAmount = ApplyDamagePawn->ParryBalanceAmount;
	float TakeHitReactPlaySecond = ApplyDamagePawn->HitReactPlaySecond;
	float StaggerTime = ETCData->SoftStaggerTime;// ETC Data
	ECombatDirection _AttackDirection = ApplyDamagePawn->AttackDirection;
	EReactionType _ReactionType = ApplyDamagePawn->AttackReaction;

	FVector HitActorLocation = GetActorLocation();
	FRotator HitActorRotation = GetActorRotation();
	FVector HitCauserLocation = DamageCauser->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HitActorLocation, HitCauserLocation);
	int Direction = FMath::RoundToInt((LookAtRotation.Yaw - HitActorRotation.Yaw - 90.f)) * -1;

	//PointDamage
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		if (PointDamageEvent)
		{
			bool bIsGuard = IsCanAction()
				&& CheckBalanceFromPawnType()
				&& CheckGuardSuccess(ApplyDamagePawn)
				&& CheckGuardableDirection(_AttackDirection, _ReactionType)
				&& ApplyDamagePawn->IsMinion() == false;

			if (bIsGuard)
			{
				if (CurrentState == ECharacterState::Parrying)
				{
					ClearDataAll();
					DoParring(_AttackDirection, _ReactionType);
					ApplyDamagePawn->OnParryHited_Func(this, TakeParryBalanceAmount);
				}
				else
				{
					GuardReaction(DamageCauser, _AttackDirection);
					CurrentState = ECharacterState::GuardHit;
				}

				if (!bIsRecoverdBalance)
					ApplyDamagePawn->ApplyDamageBalance(TakeBlockBalanceAmount);

				ApplyDamagePawn->GroggyReaction(this, Direction);
			}
			else
			{
				if (pawnType != EPawnType::Boss2)
				{
					Health = FMath::Clamp(Health - TakeDamageAmount, 0, MaxHealth);
				}
				if (!bIsRecoverdBalance || pawnType >= EPawnType::Boss1)
					ApplyDamageBalance(TakeBalanceAmount);
				
				if(mainHUD)
					mainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);

				OnHitted(DamageCauser);

				if (!HitIgnore)
				{
					ClearDataAll();

					switch (_ReactionType)
					{
					case EReactionType::ERT_LowDamage:
					{
						if (HitIgnore == false)
						{
							LowDamageReaction(DamageCauser, Direction, TakeHitReactPlaySecond);
						}
					}
					break;
					case EReactionType::ERT_HighDamage:
					{
						if (HitIgnore == false)
						{
							HighDamageReaction(DamageCauser, Direction, TakeHitReactPlaySecond);
						}
					}
					break;
					case EReactionType::ERT_GuardBreak:
					{
						HighDamageReaction(DamageCauser, Direction, TakeHitReactPlaySecond);
						StaggerTime = ETCData->HardStaggerTime;
					}
					break;
					}
				}
			}
		}
	}

	if (Health <= 0.0f)
	{
		DeathReaction(ApplyDamagePawn, Direction);
	}
	else if (balance <= 0 && pawnType > EPawnType::NormalArcher)
	{
		GroggyReaction(ApplyDamagePawn, Direction);
	}

	return DamageAmount;
}

void ABaseCharacterClass::DestroyMainWeapon()
{
	if (MainWeapon != nullptr)
		MainWeapon->Destroy();
}

bool ABaseCharacterClass::IsMinion()
{
	bool Check = (pawnType >= EPawnType::NormalSword && pawnType <= EPawnType::NormalArcher);

	return Check;
}

void ABaseCharacterClass::OnPushed()
{
}

void ABaseCharacterClass::TrackingTarget(float _Delta, float _MoveSpeed)
{
	ABaseCharacterClass* target = Cast<ABaseCharacterClass>(TargetActor);

	if (target)
	{
		FVector TargetLoc = target->GetActorLocation();
		FRotator TargetRot = target->GetActorRotation();

		if (target->CurrentState == ECharacterState::Dodge)
		{
			TargetLoc = target->BeforeDodgeLocation;
		}

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());

		LookAtRot.Pitch = 0.f;
		LookAtRot.Roll = 0.f;

		FVector Move = UKismetMathLibrary::GetForwardVector(LookAtRot) * _MoveSpeed * _Delta;

		if (AttackData)
		{
			float MyCharacterRadius = CharacterInfo->CharacterRadius;
			float TargetCharacterRadius = target->CharacterInfo->CharacterRadius;
			float DiscountValue = (AttackData->DiscountMoveAmount + (MyCharacterRadius + TargetCharacterRadius));

			float dist = FVector::Dist(target->GetActorLocation(), GetActorLocation());

			if (dist - 5.f > DiscountValue)
			{
				AddActorWorldOffset(Move,true);
			}
			else if (dist + 5.f < DiscountValue)
			{
				//LookAtRot = UKismetMathLibrary::FindLookAtRotation(TargetActor->GetActorLocation(), GetActorLocation());

				//Move = UKismetMathLibrary::GetForwardVector(LookAtRot) * _MoveSpeed * _Delta;

				AddActorWorldOffset(-Move, true);
			}
		}
		else
		{
			AddActorWorldOffset(Move, true);
		}
	}
}

void ABaseCharacterClass::TurnToTarget(float _Delta)
{
	if (bShouldTurnToTarget)
	{
		if (TargetActor)
		{
			FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());

			LookAtRot.Pitch = 0.f;
			LookAtRot.Roll = 0.f;

			SetActorRotation(LookAtRot);
		}
	}
}

bool ABaseCharacterClass::IsCanAction()
{
	return ((CurrentState == ECharacterState::Idle
		|| CurrentState == ECharacterState::Guard
		|| CurrentState == ECharacterState::GuardHit
		|| CurrentState == ECharacterState::Parrying)
		&& pawnType > EPawnType::NormalArcher);
}

bool ABaseCharacterClass::bIsDeadPawn()
{
	if (this != nullptr)
	{
		if (CurrentState == ECharacterState::Dead || GetController() == nullptr)
		{
			CurrentState = ECharacterState::Dead;
			return true;
		}
	}

	
	return false;
}

void ABaseCharacterClass::OnSwordHit(FHitResult _LastItem)
{
	ABaseCharacterClass* HitPawn = Cast<ABaseCharacterClass>(_LastItem.GetActor());
	if (HitPawn == nullptr || HitPawn->CurrentState == ECharacterState::Dead)
		return;

	if (pawnType <= EPawnType::NormalArcher && HitPawn->TeamNum != TeamNum)
	{
		GetDidItHitComponent()->ToggleTraceCheck(false);
	}

	if (HitPawn != nullptr)
	{
		if (pawnType > EPawnType::NormalArcher && HitPawn->IsMinion())
		{
			UGameplayStatics::ApplyPointDamage(HitPawn, DamageAmount * 999.f, _LastItem.ImpactNormal, _LastItem, GetController(), this, nullptr);
		}
		else
		{
			UGameplayStatics::ApplyPointDamage(HitPawn, DamageAmount, _LastItem.ImpactNormal, _LastItem, GetController(), this, nullptr);
		}
	}
}

UAnimMontage* ABaseCharacterClass::LowDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CurrentState == ECharacterState::Stun)
		return SelectedReaction;

	if (CharacterAnimationData)
	{
		if (_Direction >= 45 && _Direction <= 135)//Front
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Front")));

			ATNHCharacter* PlayerCharacter = Cast<ATNHCharacter>(_DamageCauser);

			if (PlayerCharacter && PlayerCharacter->GetIsLockOnIngaged() && IsMinion() == false)
			{
				if (CharacterAnimationData->LowDamageDir_Reaction.Num() > static_cast<int32>(PlayerCharacter->AttackDirection))
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Front-Dir %s"), *GetEnumName(PlayerCharacter->AttackDirection)));

					SelectedReaction = CharacterAnimationData->LowDamageDir_Reaction[static_cast<int32>(PlayerCharacter->AttackDirection)];

					if (!SelectedReaction)
					{
						SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Front)];
					}
				}
				else
				{
					SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Front)];
				}
			}
			else
			{
				SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Front)];
			}
		}
		else if (_Direction >= 135 && _Direction <= 225)//Left
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Left")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Left)];
		}
		else if (_Direction >= 255 && _Direction <= 315)//Rear
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Rear")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Rear)];
		}
		else//Right
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Right")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Right)];
		}

		if (SelectedReaction)
		{
			float PlayRate = 1.f;

			if (_PlaySecond > 0)
			{
				float AnimTime = SelectedReaction->GetPlayLength();
				PlayRate = AnimTime / _PlaySecond;
			}

			ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(_DamageCauser);
			if (IsMinion() && BaseCharacter->IsMinion())
			{
				PlayRate = 1.f;
			}

			GetCharacterAnimInstance()->StopAllMontages(0.1f);

			GetCharacterAnimInstance()->Montage_Play(SelectedReaction, PlayRate);
		}
	}

	return SelectedReaction;
}

UAnimMontage* ABaseCharacterClass::GuardBreakReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CharacterAnimationData)
	{
		if (_Direction >= 90.f - GuardDegree && _Direction <= 90.f + GuardDegree)//GuardBreak
		{
			SelectedReaction = CharacterAnimationData->GuardBreak_Reaction;
		}
		else if (_Direction >= 135 && _Direction <= 225)//Left
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Left")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Left)];
		}
		else if (_Direction >= 255 && _Direction <= 315)//Rear
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Rear")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Rear)];
		}
		else//Right
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Right")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction[static_cast<int32>(EHitDirection::Right)];
		}

		if (SelectedReaction)
		{
			float PlayRate = 1.f;

			if (_PlaySecond > 0)
			{
				float AnimTime = SelectedReaction->GetPlayLength();
				PlayRate = AnimTime / _PlaySecond;
			}

			GetCharacterAnimInstance()->Montage_Play(SelectedReaction, PlayRate);
		}
	}

	return SelectedReaction;
}

UAnimMontage* ABaseCharacterClass::HighDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedReaction = nullptr;
	if (CurrentState == ECharacterState::Stun)
		return SelectedReaction;

	if (CharacterAnimationData)
	{
		if (_Direction >= 45 && _Direction <= 135)//Front
		{
			SelectedReaction = CharacterAnimationData->HighDamage_Reaction[static_cast<int32>(EHitDirection::Front)];
		}
		else if (_Direction >= 135 && _Direction <= 225)//Left
		{
			SelectedReaction = CharacterAnimationData->HighDamage_Reaction[static_cast<int32>(EHitDirection::Left)];
		}
		else if (_Direction >= 255 && _Direction <= 315)//Rear
		{
			SelectedReaction = CharacterAnimationData->HighDamage_Reaction[static_cast<int32>(EHitDirection::Rear)];
		}
		else//Right
		{
			SelectedReaction = CharacterAnimationData->HighDamage_Reaction[static_cast<int32>(EHitDirection::Right)];
		}

		if (SelectedReaction)
		{
			float PlayRate = 1.f;

			if (_PlaySecond > 0)
			{
				float AnimTime = SelectedReaction->GetPlayLength();
				PlayRate = AnimTime / _PlaySecond;
			}

			GetCharacterAnimInstance()->StopAllMontages(0.1f);

			GetCharacterAnimInstance()->Montage_Play(SelectedReaction, PlayRate);
		}
	}

	return SelectedReaction;
}

UAnimMontage* ABaseCharacterClass::KnockBackReaction(AActor* _DamageCauser, FVector _DirVector)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CharacterAnimationData)
	{
		FVector Forward = _DamageCauser->GetActorForwardVector();
		FVector Up = GetActorUpVector();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _DamageCauser->GetActorLocation());

		FVector Result;

		Forward = Forward * _DirVector.X;
		Up = Up * _DirVector.Z;

		Result = Forward + Up;

		SetActorRotation(LookAtRotation);
		LaunchCharacter(Result, false, false);

		int size = CharacterAnimationData->KnockBack_Reaction.Num();
		int randNum = UKismetMathLibrary::RandomInteger(size);

		SelectedReaction = CharacterAnimationData->KnockBack_Reaction[randNum];

		if (SelectedReaction)
		{
			CurrentKnockBackAnim = SelectedReaction;
			GetCharacterAnimInstance()->Montage_Play(CurrentKnockBackAnim);
		}
	}

	return SelectedReaction;
}

UAnimMontage* ABaseCharacterClass::GuardReaction(AActor* _DamageCauser, ECombatDirection _Direction)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CharacterAnimationData)
	{
		int index = FMath::Clamp(static_cast<uint32>(_Direction), 0, 1);

		if (CharacterAnimationData->Guard_Reaction.Num() > index)
		{
			SelectedReaction = CharacterAnimationData->Guard_Reaction[index];

			if (SelectedReaction)
			{
				GetCharacterAnimInstance()->Montage_Play(SelectedReaction);

				OnAttackBlocked.Broadcast();
			}
		}
	}

	return SelectedReaction;
}

UAnimMontage* ABaseCharacterClass::DeathReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (pawnType >= EPawnType::Boss1)
		return nullptr;

	if (CharacterAnimationData)
	{
		if (_Direction >= 45 && _Direction <= 135)//Front
		{
			SelectedReaction = CharacterAnimationData->Death[static_cast<int32>(EHitDirection::Front)];
		}
		else if (_Direction > 135 && _Direction <= 225)//Left
		{
			SelectedReaction = CharacterAnimationData->Death[static_cast<int32>(EHitDirection::Left)];
		}
		else if (_Direction > 255 && _Direction <= 315)//Rear
		{
			SelectedReaction = CharacterAnimationData->Death[static_cast<int32>(EHitDirection::Rear)];
		}
		else//Right
		{
			SelectedReaction = CharacterAnimationData->Death[static_cast<int32>(EHitDirection::Right)];
		}

		DestroyMainWeapon();

		if (SelectedReaction)
		{
			Health = 0.f;
			StopAnimMontage();
			GetCharacterAnimInstance()->Montage_Play(SelectedReaction);
		}
	}

	return SelectedReaction;
}

void ABaseCharacterClass::ClearDataAll()
{
	//Data Clear
	GetCharacterAnimInstance()->StopAllMontages(0.0f);
	CurrentComboIndex = 0;
}

UAnimMontage* ABaseCharacterClass::GroggyReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (balance > 0 )
		return SelectedReaction;

	if (IsMinion()) return DeathReaction(_DamageCauser, _Direction);

	if (CharacterAnimationData)
	{
		GetCharacterAnimInstance()->StopAllMontages(1.0);

		int size = CharacterAnimationData->Stun_Reaction.Num();
		int randNum = UKismetMathLibrary::RandomInteger(size);

		SelectedReaction = CharacterAnimationData->Stun_Reaction[randNum];

		if (SelectedReaction)
		{
			GetCharacterAnimInstance()->Montage_Play(SelectedReaction);
		}
	}

	return SelectedReaction;
}

void ABaseCharacterClass::DoParring(ECombatDirection _Direction, EReactionType _ReactionType)
{
}

void ABaseCharacterClass::KnockBackCheckRagdoll()
{
	if (GetMovementState() == EALSMovementState::Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
}

bool ABaseCharacterClass::CheckBalanceFromPawnType()
{
	switch (pawnType)
	{
	case EPawnType::Player:
	{
		return true;
	}
	default:
	{
		if (balance > 0)
			return true;
		else
			return false;
	}
	}

	return false;
}

void ABaseCharacterClass::InitNextAttackData(int32 _Index, ECombatDirection _Direction, bool _bTargetExist)
{
	AttackData = GetAttackInfoData(_Index, _Direction, _bTargetExist);

	if (AttackData)
	{
		AttackReaction = AttackData->ReactionType;
		AttackDirection = AttackData->AttackDirection;
		DamageAmount = AttackData->Damage;
		BalanceAmount = AttackData->BalanceDamage;
		BlockBalanceAmount = AttackData->BlockBalance;
		ParryBalanceAmount = AttackData->ParryBalance;
		HitReactPlaySecond = AttackData->HitReactPlaySecond;
	}
}

void ABaseCharacterClass::InitMotionWarpingData(AActor* _AttackTarget)
{
	bStartTracking = false;
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), GetActorLocation(), GetActorRotation());

	if (_AttackTarget)
	{
		TrackAttackTarget = Cast<ABaseCharacterClass>(_AttackTarget);

		if (TrackAttackTarget && AttackData)
		{
			float dist = FVector::Dist(TrackAttackTarget->GetActorLocation(), GetActorLocation());
			float MyCharacterRadius = CharacterInfo->CharacterRadius;
			float TargetCharacterRadius = TrackAttackTarget->CharacterInfo->CharacterRadius;

			float DiscountValue = (AttackData->DiscountMoveAmount + (MyCharacterRadius + TargetCharacterRadius));

			MoveDistance = (dist >= MinTrackDistance) ? AttackTrackDist : (dist <= MaxTrackDistance) ? AttackTrackDist : 0.f;

			MoveDistance = FMath::Clamp(MoveDistance, DiscountValue, AttackTrackDist);

			FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TrackAttackTarget->GetActorLocation());
			FVector TargetLoc = (GetActorLocation() + UKismetMathLibrary::GetForwardVector(TargetRot) * MoveDistance);

			TargetRot.Pitch = GetActorRotation().Pitch;

			if (TrackAttackTarget->CurrentState == ECharacterState::Dodge)
			{
				TargetLoc = TrackAttackTarget->BeforeDodgeLocation;
			}
			else if (dist <= MoveDistance + DiscountValue)
			{
				TargetLoc = TrackAttackTarget->GetActorLocation() + 
					(UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(TrackAttackTarget->GetActorLocation(), GetActorLocation())) * DiscountValue);

				bStartTracking = true;
			}

			OriginTrackingPosition = GetActorLocation();

			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), TargetLoc, TargetRot);
		}
	}
}

void ABaseCharacterClass::UpdateAttackLocation()
{
	if (bStartTracking && TrackAttackTarget && AttackData)
	{
		float dist = FVector::Dist(TrackAttackTarget->GetActorLocation(), GetActorLocation());			
		float MyCharacterRadius = CharacterInfo->CharacterRadius;
		float TargetCharacterRadius = TrackAttackTarget->CharacterInfo->CharacterRadius;

		float DiscountValue = (AttackData->DiscountMoveAmount + (MyCharacterRadius + TargetCharacterRadius));
		
		float CurrentMoveDist = FVector::Dist(GetActorLocation(), OriginTrackingPosition);
		
		//Moved Distance is not over AttackTrackDist
		if (CurrentMoveDist <= AttackTrackDist)
		{
			if (TrackAttackTarget->CurrentState == ECharacterState::Dodge)
			{
				FVector TargetLoc = TrackAttackTarget->BeforeDodgeLocation;

				MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), TargetLoc, GetActorRotation());
			}
			else if (dist <= MoveDistance)
			{
				FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TrackAttackTarget->GetActorLocation());

				FVector TargetLoc = TrackAttackTarget->GetActorLocation() +
					(UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(TrackAttackTarget->GetActorLocation(), GetActorLocation())) * DiscountValue);

				MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), TargetLoc, TargetRot);
				//SetActorLocationAndRotation(TargetLoc, TargetRot);
			}
			else
			{
				StopAttackTracking();
			}
		}
		else
		{
			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), GetActorLocation(), GetActorRotation());

			StopAttackTracking();
		}
	}
}

void ABaseCharacterClass::StopAttackTracking()
{
	if (bStartTracking)
	{
		bStartTracking = false;
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), GetActorLocation(), GetActorRotation());
	}
}

void ABaseCharacterClass::SetLockedCollisionRadius(ABaseCharacterClass* _Target, bool _Locked)
{
	if (_Target)
	{
		if (_Locked)
		{
			_Target->GetCapsuleComponent()->SetCapsuleRadius(CapsuleLockOnRadius);
		}
		else
		{
			_Target->GetCapsuleComponent()->SetCapsuleRadius(CapsuleNormalRadius);
		}
	}
}

void ABaseCharacterClass::SettingStatusPawn()
{
	UTNHGameInstance* GameInstance = Cast<UTNHGameInstance>(GetGameInstance());
	if (GameInstance != nullptr)
	{
		CharacterInfo = GameInstance->CharacterStatusDataTable->FindRow<FCharacterStatusTableRow>(FName(GetPawnType()), "");

		MaxHealth = CharacterInfo->MaxHealth;
		Maxbalance = CharacterInfo->MaxBalance;
		Walkspeed = CharacterInfo->WalkSpeed;
		runSpeed = CharacterInfo->RunSpeed;
		DefaultThreat = CharacterInfo->DefaultThreat;
		Health = MaxHealth;
		balance = Maxbalance;
		GetCharacterMovement()->MaxWalkSpeed = CharacterInfo->WalkSpeed;

		SettingPawnDataTable();
	}
}

void ABaseCharacterClass::InitData()
{
	SettingStatusPawn();

	if (GetDidItHitComponent())
	{
		if (GetDidItHitComponent()->OnItemAdded.Contains(this, FName(TEXT("OnSwordHit"))) == false)
		{
			GetDidItHitComponent()->OnItemAdded.AddDynamic(this, &ThisClass::OnSwordHit);
		}
	}

	if(_TargetSelectComponents != nullptr)
		_TargetSelectComponents->SetOwner(this);

	Stimulisoure->RegisterForSense(TSubclassOf<UAISense>());
	SpawnMainWeapon();
	SetColiisionSetting();
}

void ABaseCharacterClass::SetColiisionSetting()
{
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleNormalRadius);
	GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacterClass::SpawnMainWeapon()
{
	if (SpawnWeaponClass)
	{
		FActorSpawnParameters Params;

		Params.Instigator = this;
		Params.Owner = this;

		AWeaponBase* TempWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnWeaponClass, GetActorTransform(), Params);
		TempWeapon->SetEquipSocket("LongSwordHandSocket");
		TempWeapon->OnInit();

		MainWeapon = TempWeapon;
		MainWeapon->OnEquipped();

		if (IsValid(MainWeapon))
		{
			GetDidItHitComponent()->SkipStringFilter = "LeftHand";
			TArray<FName> Sockets = GetDidItHitComponent()->SetupVariables(MainWeapon->GetItemMesh(), nullptr);
		}
	}
}

FAttackRow* ABaseCharacterClass::GetAttackInfoData(int _index, ECombatDirection _Direction, bool _bTargetExist)
{
	FString TempStr = GetPawnType();
	if (pawnType == EPawnType::Player)
	{
		if (_bTargetExist == true)
		{
			if (_index >= 0)
			{
				TempStr.Append("_Attack_");
				TempStr.Append(FString::FromInt(_index));
				TempStr.Append("_");
				TempStr.Append((_Direction == ECombatDirection::Left) ? "L" : "R");
			}
			else if (_index == -1)
			{
				TempStr.Append("_Crash");
			}
		}
		else
		{
			if (_index >= 0)
			{
				TempStr.Append("_Attack_");
				TempStr.Append(FString::FromInt(_index));
				TempStr.Append("_NoTarget");
			}
		}
	}

	return AttackInfoData->FindRow<FAttackRow>(*TempStr, "");
}

void ABaseCharacterClass::SettingPawnDataTable()
{
	UTNHGameInstance* GameInstance = Cast<UTNHGameInstance>(GetGameInstance());

	CharacterAnimationData = GameInstance->AnimationDataTable->FindRow<FAnimationTable>(FName(GetPawnType()), "");
	AttackInfoData = GameInstance->AttackDataTable;
	ExecutionInfoData = GameInstance->ExecutionDataTable;
	ETCTable = GameInstance->ETCTable;
	ETCData = ETCTable->FindRow<FDT_ETC>(FName(GetPawnType()), "");
}

float ABaseCharacterClass::GetRecoveredTime()
{
	return CharacterInfo->RT_Balance;
}

float ABaseCharacterClass::GetCurrentRecoveredTime()
{
	return RecoveredTime;
}

void ABaseCharacterClass::ApplyDamageBalance(float Damage)
{
	if (balance - Damage >= 0)
	{
		balance -= Damage;
		RecoveredTime = CharacterInfo->RT_Balance;
		
	}
	else
	{
		balance = 0;
		RecoveredTime = CharacterInfo->RT_Balance;
		bIsRecoverdBalance = true;
		//CurrentState = ECharacterState::Excution;
	}

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);

}

float ABaseCharacterClass::GetRecvoreValue()
{
	return CharacterInfo->RV_Balance;
}

void ABaseCharacterClass::RecoverdBalace()
{
	if (CurrentState == ECharacterState::Dead || CurrentState == ECharacterState::Stun || balance == Maxbalance)
		return;

	if (balance < Maxbalance)
	{
		if (balance + CharacterInfo->RV_Balance * FApp::GetDeltaTime() >= Maxbalance)
		{
			balance = Maxbalance;
		}
		else
		{
			balance += CharacterInfo->RV_Balance * FApp::GetDeltaTime();
		}
	}

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
}

FString ABaseCharacterClass::GetPawnType()
{
	if (UEnum::GetDisplayValueAsText(pawnType).ToString() != "")
		return  UEnum::GetDisplayValueAsText(pawnType).ToString();

	return "";
}

void ABaseCharacterClass::SetFixedRagdoll()
{
	FTimerHandle TimerHandle;
	RagdollStart();
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::FixRagdoll, 6.f, false);
}

void ABaseCharacterClass::SetCharacterWalkSpeed(float _Value)
{
	GetCharacterMovement()->MaxWalkSpeed = _Value;
}

void ABaseCharacterClass::PlayGuardAnim(bool _End)
{
	UAnimMontage* GuardAnim = CharacterAnimationData->Guard;

	if (GuardAnim)
	{
		if (_End == false)
		{
			GetCharacterAnimInstance()->Montage_Play(GuardAnim);
		}
		else
		{
			GetCharacterAnimInstance()->Montage_JumpToSection(FName("End"), GuardAnim);
		}
	}
}

void ABaseCharacterClass::ResumeGuardAnim(UAnimMontage* _Montage, bool _bInterrupt)
{
	UAnimMontage* GuardAnim = CharacterAnimationData->Guard;

	if (_bInterrupt == false)
	{
		if (GuardAnim && IsKeepGuard)
		{
			GetCharacterAnimInstance()->Montage_Play(GuardAnim);
			GetCharacterAnimInstance()->Montage_JumpToSection(FName("Loop"), GuardAnim);
		}
	}
}

void ABaseCharacterClass::FindExecutionTarget()
{
	FVector CharacterLocation = GetActorLocation();

	Execution_Target = nullptr;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Emplace(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<FHitResult> HitResults;

	if (UKismetSystemLibrary::SphereTraceMultiForObjects(this, CharacterLocation, CharacterLocation, 300.f, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResults, true))
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (CanExecution(HitResult.GetActor()))
			{
				if (Execution_Target == nullptr)
				{
					Execution_Target = Cast<ABaseCharacterClass>(HitResult.GetActor());
				}
				else
				{
					float DistA = FVector::Distance(Execution_Target->GetActorLocation(), GetActorLocation());
					float DistB = FVector::Distance(HitResult.GetActor()->GetActorLocation(), GetActorLocation());

					if (DistA > DistB)
					{
						Execution_Target = Cast<ABaseCharacterClass>(HitResult.GetActor());
					}
				}
			}
		}
	}
}

bool ABaseCharacterClass::CanExecution(AActor* _Target)
{
	ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(_Target);

	//&& ((Target->GetActorLocation() - GetActorLocation()).GetAbs().Z) <= 44.f

	if (Target)
	{
		bool check = Target->TeamNum != TeamNum
			&& Target->CurrentState != ECharacterState::Dead
			&& (Target->CurrentState == ECharacterState::Stun
				|| (Target->pawnType >= EPawnType::NormalSword && Target->pawnType <= EPawnType::NormalArcher));

		return check;
	}

	return false;
}

void ABaseCharacterClass::Execution_Play(int32 _index)
{
	if (Execution_Target == nullptr) return;

	//if index -1, montage random play
	if (_index == -1)
	{
		FString ExecutionRow;

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Execution_Target->GetActorLocation());

		int Direction = FMath::RoundToInt((LookAtRotation.Yaw - GetActorRotation().Yaw - 90.f)) * -1;

		switch (Execution_Target->pawnType)
		{
		case EPawnType::NormalSword:
		case EPawnType::NormalSpear:
		case EPawnType::NormalGladiator:
		case EPawnType::NormalArcher:
		{
			ExecutionRow.Append(L"Normal_");
		}
		break;
		case EPawnType::EliteSword:
		case EPawnType::EliteSpear:
		case EPawnType::EliteGladiator:
		case EPawnType::HighEliteSword:
		case EPawnType::HighEliteSpear:
		case EPawnType::HighEliteGladiator:
		{
			ExecutionRow.Append(L"Elite_");
		}
		break;
		case EPawnType::Boss1:
		{
			ExecutionRow.Append(L"Boss1_");
		}
		break;
		}

		if (Direction >= 45 && Direction <= 135)//Front
		{
			ExecutionRow.Append(L"F_");
		}
		else if (Direction >= 135 && Direction <= 225)//Left
		{
			ExecutionRow.Append(L"L_");
		}
		else if (Direction >= 255 && Direction <= 315)//Rear
		{
			ExecutionRow.Append(L"B_");
		}
		else//Right
		{
			ExecutionRow.Append(L"R_");
		}

		int MaxRandNum = 0;
		ExecutionRow.Append(FString::FromInt(FMath::RandRange(0, MaxRandNum)));
	}
}

void ABaseCharacterClass::Execution_Attacker(int32 _index)
{
}

void ABaseCharacterClass::Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance)
{
	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ABaseCharacterClass::OnEnded_Victim);

	//GetMesh()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ABaseCharacterClass* Attacker = Cast<ABaseCharacterClass>(_ExecutionAttacker);
	ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(Attacker->TargetActor);

	//FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(_ExecutionAttacker->GetActorLocation(), GetActorLocation());
	FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _ExecutionAttacker->GetActorLocation());
	FVector NextLocation = _ExecutionAttacker->GetActorLocation();// + _ExecutionAttacker->GetActorForwardVector() * _BetweenDistance

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;
	CurrentState = ECharacterState::Dead;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), GetActorLocation(), NextRotation, false, false, 0.2f, false,
		EMoveComponentAction::Move, LatentInfo);

	GetCharacterAnimInstance()->Montage_Play(_VictimMontage);
		
	GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, _VictimMontage);
}

void ABaseCharacterClass::OnEnded_Attacker(UAnimMontage* _VictimMontage, bool _Interrupt)
{
}

void ABaseCharacterClass::OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt)
{
	//GetMesh()->bPauseAnims = true;
	//GetMesh()->GetAnimInstance()->Montage_Pause(_VictimMontage);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//GetMesh()->SetCollisionProfileName(FName("CharacterMesh"));
	//GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));

	//LaunchCharacter(FVector(0.f, 0.f, 50.f), false, true);

	//GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);

	//RagdollStart();

	Health = 0.f;

	//CurrentState = ECharacterState::Dead;
}

void ABaseCharacterClass::OnEnded_ParryHitReaction(UAnimMontage* _HitReaction, bool _Interrupt)
{
	if (LastestApplyDamagePawn)
	{
		FVector HitActorLocation = GetActorLocation();
		FRotator HitActorRotation = GetActorRotation();
		FVector HitCauserLocation = LastestApplyDamagePawn->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HitActorLocation, HitCauserLocation);
		int Direction = FMath::RoundToInt((LookAtRotation.Yaw - HitActorRotation.Yaw - 90.f)) * -1;

		if (balance <= 0 && pawnType > EPawnType::NormalArcher)
		{
			GroggyReaction(LastestApplyDamagePawn, Direction);
		}
		else if (CharacterAnimationData && CharacterAnimationData->ParryStun_Reaction)
		{
			FOnMontageEnded OnEnded;

			OnEnded.BindUObject(this, &ABaseCharacterClass::OnEnded_ParryStunReaction);

			GetCharacterAnimInstance()->Montage_Play(CharacterAnimationData->ParryStun_Reaction);
			GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, CharacterAnimationData->ParryStun_Reaction);
		}
	}
}

void ABaseCharacterClass::OnEnded_ParryStunReaction(UAnimMontage* _HitReaction, bool _Interrupt)
{ }

void ABaseCharacterClass::ChangeControllerRotation(bool flag)
{
	if (flag)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

FString ABaseCharacterClass::GetAttackName()
{
	FString TempStr = GetPawnType();

	int _index = FMath::RandRange(0, 3);

	TempStr.Append("_Attack_");
	TempStr.Append(FString::FromInt(_index));

	return TempStr;
}
