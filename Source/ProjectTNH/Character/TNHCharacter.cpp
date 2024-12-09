// Fill out your copyright notice in the Description page of Project Settings.

#include "TNHCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "../PlayerController/TNHPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../UI/HUD/TNHMainHUD.h"
#include "Components/WidgetComponent.h"
#include "../UI/WB_StateBar.h"
#include "Components/SphereComponent.h"
#include "ProjectTNH/AI/AI_DataTable/DT_AiETCDataRow.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectTNH/Components/CombatComponent.h"
#include "Engine/EngineTypes.h"
#include "ProjectTNH/ProjectTNH.h"
#include "ProjectTNH/DataTable/DT_AnimationTable.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/CameraSystem/TNHPlayerCameraBehavior.h"
#include "../UI/CustomWidget_PlayerStateBarBase.h"
#include "ProjectTNH/DamageType/DT_LongSwordAttack.h"
#include "Engine/DamageEvents.h"
#include "ProjectTNH/DataTable/DT_AttackRow.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/BoxComponent.h"
#include "Character/ALSPlayerCameraManager.h"
#include "ProjectTNH/UI/DirectionIndicator_Widget.h"
#include "ProjectTNH/CameraSystem/SwordCameraShake.h"
#include "Components/PostProcessComponent.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "MotionWarping.h"
#include "../AI/UAiCommander.h"
#include "MotionWarpingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Boss_CharacterBase.h"
#include "ProjectTNH/AI/Ai_CharacterBase/BossSecondPhase.h"

ATNHCharacter::ATNHCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	HPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(GetMesh());

	sphereRange = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColision"));
	sphereRange->SetupAttachment(RootComponent);

	pawnType = EPawnType::Player;

	DamagePostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("DamagePostProcess"));
	DamagePostProcess->SetupAttachment(GetCapsuleComponent());
}

void ATNHCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HPBar)
	{
		HPBar->SetVisibility(false);
	}

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ATNHCharacter::BeginPlay()
{
	Super::BeginPlay();

	//GetDidItHitComponent()->MyDrawDebugType = EDrawDebugTrace::Persistent;

	InitalizeFSM();

	Ownnigcontroller = Cast<ATNHPlayerController>(GetController());

	UTNHPlayerCameraBehavior* TNHCameraBehavior = Cast<UTNHPlayerCameraBehavior>(CameraBehavior);

	if (TNHCameraBehavior)
	{
		TNHCameraBehavior->OnInit(this);
	}

	OnHited.AddUObject(this, &ThisClass::OnHited_Func);
	OnLowHP.AddUObject(this, &ThisClass::OnLowHP_Func);
	OnEnoughHP.AddUObject(this, &ThisClass::OnEnoughHP_Func);

	SetDesiredGait(EALSGait::Running);

	SetOverlayState(EALSOverlayState::SwordTwoHanded);
	GetMainWeapon()->AttachWeapon(FName("SwordHoldSocket"));

	OnTargetFinded.AddUObject(this, &ThisClass::OnTargetFinded_Func);
	OnTargetLosted.AddUObject(this, &ThisClass::OnTargetLosted_Func);

	InitalizeExecutionData();
}

void ATNHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//
	// 
	// ->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Current State : %s"), *GetEnumDisplayName(CurrentState)));

	ForwardScale = 0.f;
	RightScale = 0.f;

	PlayerStateMachine.Update(DeltaTime);

	
	UpdateLockOn();

	if (IsLockOnIngaged == false 
		&& !(IsPlayingExecution == true && EliteExecution == true))
	{
		FindExecutionTarget();
	}

	UpdateHealthRegen(DeltaTime);
	
	UpdateDirectionIndicator();
}

float ATNHCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AALSCharacter::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (EventInstigator == nullptr)
		return 0.0f;

	if (CurrentState == ECharacterState::Dead
		|| (CurrentState==ECharacterState::Execution && EliteExecution)
		|| bIsSuperArmor) return 0.0f;

	UDT_LongSwordAttack* DamageType = Cast<UDT_LongSwordAttack>(DamageEvent.DamageTypeClass->GetDefaultObject());
	ABaseCharacterClass* ApplyDamagePawn = Cast<ABaseCharacterClass>(EventInstigator->GetPawn());

	LastBeAttackedCharacter = ApplyDamagePawn;

	float TakeDamageAmount = Damage;
	float TakeBalanceAmount = ApplyDamagePawn->BalanceAmount;
	float TakeBlockBalanceAmount = ApplyDamagePawn->BlockBalanceAmount;
	float TakeParryBalanceAmount = ApplyDamagePawn->ParryBalanceAmount;
	float TakeHitReactPlaySecond = ApplyDamagePawn->HitReactPlaySecond;
	float StaggerTime = ETCData->SoftStaggerTime;// ETC Data
	ECombatDirection _AttackDirection = ApplyDamagePawn->AttackDirection;
	EReactionType _ReactionType = ApplyDamagePawn->AttackReaction;

	float Dotvalue = UKismetMathLibrary::Dot_VectorVector(DamageCauser->GetActorForwardVector(), GetActorForwardVector());
	if (ApplyDamagePawn->TeamNum == TeamNum)
		return 0.0f;

	FVector HitActorLocation = GetActorLocation();
	FRotator HitActorRotation = GetActorRotation();
	FVector HitCauserLocation = DamageCauser->GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HitActorLocation, HitCauserLocation);

	int Direction = FMath::Clamp(FMath::RoundToInt((LookAtRotation.Yaw - HitActorRotation.Yaw - 90.f)) * -1, 0, 360);

	ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(Ownnigcontroller->GetHUD());

	//PointDamage
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		if (PointDamageEvent)
		{
			if (IsCanAction()
				&& CheckBalanceFromPawnType()
				&& CheckGuardSuccess(ApplyDamagePawn)
				&& CheckGuardableDirection(_AttackDirection, _ReactionType)
				&& ApplyDamagePawn->IsMinion() == false
				&& IsLockOnIngaged)
			{
				UCustomWidget_PlayerStateBarBase* PlayerStateBar = Cast< UCustomWidget_PlayerStateBarBase>(GameMainHUD->GetWidgetInHud("PlayerStateBar"));
				GameMainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);

				ClearDataAll();

				if (CurrentState == ECharacterState::Parrying)
				{
					DoParring(_AttackDirection, _ReactionType);
					ApplyDamagePawn->OnParryHited_Func(this, TakeParryBalanceAmount);

					UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(ParryCameraShake);

					PlayerStateBar->OnPlayerParried();
				}
				else
				{
					GuardReaction(DamageCauser, _AttackDirection);

					DisableDodge = true;

					UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(BlockCameraShake);
					PlayerStateBar->OnPlayerGuarded();
				}

				if (!bIsRecoverdBalance)
					ApplyDamagePawn->ApplyDamageBalance(TakeBlockBalanceAmount);

				ApplyDamagePawn->GroggyReaction(this, Direction);
			}
			else
			{
				if (BloodEffectAry.Num() > 0)
				{
					auto EffectNum = FMath::RandRange(0, BloodEffectAry.Num() - 1);

					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						this, BloodEffectAry[EffectNum], PointDamageEvent->HitInfo.ImpactPoint,
						FRotator(0.f, 0.f, 0.f), FVector(1.0f, 1.0f, 1.0f), true,
						true, ENCPoolMethod::None, true);
				}

				if (CurrentState == ECharacterState::Dodge && RollDodge == true)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Roll Dodged")));
				}
				else if (pawnType > EPawnType::NormalArcher 
					&& CurrentState == ECharacterState::Dodge
					&& _AttackDirection==ECombatDirection::Crash)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Avoid Attack")));
				}
				else
				{
					UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(HitCameraShake);

					Health = FMath::Clamp(Health - TakeDamageAmount, MinHealth, MaxHealth);

					ResetRegenTimer();

					if (!bIsRecoverdBalance)
						ApplyDamageBalance(TakeBalanceAmount);

					UCustomWidget_PlayerStateBarBase* PlayerStateBar = Cast< UCustomWidget_PlayerStateBarBase>(GameMainHUD->GetWidgetInHud("PlayerStateBar"));
					PlayerStateBar->OnPlayerHited();
					Super::OnHitted(DamageCauser);

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
					case EReactionType::ERT_KnockBack:
					{
					}
					break;
					case EReactionType::ERT_GuardBreak:
					{
						HighDamageReaction(DamageCauser, Direction, TakeHitReactPlaySecond);
						StaggerTime = ETCData->HardStaggerTime;
					}
					break;
					case EReactionType::ERT_Execution:
					{
					}
					break;
					case EReactionType::ERT_Stun:
						break;
					}
				}
			}
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);

		if (RadialDamageEvent)
		{
		}
	}

	GameMainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);

	if (Health <= 0)
	{
		ClearDataAll();
		DeathReaction(ApplyDamagePawn, Direction);
	}

	return DamageAmount;
}

void ATNHCharacter::FindExecutionTarget()
{
	FVector FindLocation = GetActorLocation();
	FRotator FindRotation = GetActorRotation();

	ABaseCharacterClass* SavedExecution_Target = Execution_Target;

	Execution_Target = nullptr;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Emplace(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<FHitResult> HitResults;

	AALSPlayerCameraManager* CameraManager = Cast<AALSPlayerCameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get());

	if (CameraManager)
	{
		if (ExecutionFindMode)
		{
			FindLocation = CameraManager->GetCameraLocation();
		}
		FindRotation = CameraManager->GetCameraRotation();
	}

	if (UKismetSystemLibrary::SphereTraceMultiForObjects(this, GetActorLocation(), GetActorLocation(), ExecutionFindDist, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResults, true))
	{
		float CurrentTargetScore = 0;
		TArray<AActor*> NearlyTeam;

		for (const FHitResult& HitResult : HitResults)
		{
			ABaseCharacterClass* TempTarget = Cast<ABaseCharacterClass>(HitResult.GetActor());

			if (TempTarget && TempTarget->TeamNum == TeamNum)
			{
				NearlyTeam.AddUnique(TempTarget);
			}
		}

		for (const FHitResult& HitResult : HitResults)
		{
			ABaseCharacterClass* TempTarget = Cast<ABaseCharacterClass>(HitResult.GetActor());

			if (TempTarget)
			{
				bool CanTargetCheck = TempTarget->TeamNum != TeamNum
					&& (TempTarget->GetActorLocation() - GetActorLocation()).GetAbs().Z <= 100.f
					&& TempTarget->CurrentState != ECharacterState::Dead;

				if (CanTargetCheck == false) continue;

				FVector Direction = (TempTarget->GetActorLocation() - FindLocation).GetSafeNormal();
				FVector Forward = UKismetMathLibrary::GetForwardVector(FindRotation);

				float BetweenAngle = FMath::Acos(FVector::DotProduct(Forward, Direction));

				float BetweenDegree = FMath::RadiansToDegrees(BetweenAngle);

				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Degree %f"), BetweenDegree));

				if (BetweenDegree >= 90.f) continue;

				float Target_DistScore = FVector::Distance(TempTarget->GetActorLocation(), FindLocation) * ExecutionDistanceApplyPoint * -1;
				float Target_AngleScore = (BetweenDegree * ExecutionAngleApplyPoint) * -1;
				float Target_TypeScore = (TempTarget->pawnType >= EPawnType::EliteSword
					&& TempTarget->pawnType <= EPawnType::HighEliteGladiator) ? ExecutionEliteApplyPoint : 0.f;

				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Score %f %f %f"), Target_DistScore, Target_AngleScore, Target_TypeScore));

				float TempTargetScore = Target_DistScore + Target_AngleScore + Target_TypeScore;

				FCollisionQueryParams Params;

				Params.AddIgnoredActor(this);

				TEnumAsByte<ETraceTypeQuery> TraceType = UEngineTypes::ConvertToTraceType(ECC_WorldStatic);

				FHitResult CornerHitResult;

				TArray<AActor*> Ignore;
				Ignore.AddUnique(TempTarget);

				for (auto Team : NearlyTeam)
				{
					Ignore.AddUnique(Team);
				}

				bool bHit = UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), TempTarget->GetActorLocation(), TraceType,
					false, Ignore, EDrawDebugTrace::None, CornerHitResult, true);

				if (bHit == false)
				{
					if (Execution_Target == nullptr)
					{
						Execution_Target = TempTarget;

						CurrentTargetScore = TempTargetScore;
					}
					else if (CurrentTargetScore < TempTargetScore)
					{
						Execution_Target = TempTarget;
					}
				}
			}
		}

		//Execution target is exist and find new ExecutionTarget
		if (Execution_Target && SavedExecution_Target != Execution_Target)
		{
			if (ExecutionTargetUpdateOnce.DoOnceFlag == false)
				ExecutionTargetUpdateOnce.Reset();

			OnExecutionTargetUpdated(Execution_Target);
		}
		else if (Execution_Target == nullptr && ExecutionTargetUpdateOnce.Execute())
		{
			OnExecutionTargetUpdated(nullptr);
		}
	}
}

void ATNHCharacter::GetBoxCorners(FVector _Origin, FVector _BoxExtent, TArray<FVector>& Corners)
{
	FVector CornerLocation = _Origin;
	Corners.Add(CornerLocation);
}

void ATNHCharacter::DisableRootMotion(float _DisableRootMotionTime)
{
	FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
	if (LastInputVector.Equals(FVector::ZeroVector, 0.0001f))
	{
		GetCharacterAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);

		GetWorldTimerManager().SetTimer(
			ResetRootMotionTimer,
			this,
			&ATNHCharacter::EnableRootMotion,
			_DisableRootMotionTime
		);
	}
}

void ATNHCharacter::EnableRootMotion()
{
	GetCharacterAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
}

void ATNHCharacter::ResetRegenTimer()
{
	if (GetWorldTimerManager().IsTimerActive(ResetRootMotionTimer)) 
		GetWorldTimerManager().ClearTimer(ResetRootMotionTimer);

	bHealthRegen = false;

	GetWorldTimerManager().SetTimer(
		ResetRootMotionTimer,
		this,
		&ATNHCharacter::StartRegen,
		CharacterInfo->RT_Health
	);
}

void ATNHCharacter::StartRegen()
{
	bHealthRegen = true;
}

FRotator ATNHCharacter::GetDesiredRotation()
{
	//Check Character has Input
	if (!GetCharacterMovement()->GetLastInputVector().Equals(FVector::ZeroVector, 0.001f))
	{
		//Get Last InputVector Rotation
		return UKismetMathLibrary::MakeRotFromX(GetLastMovementInputVector());
	}

	//Character has no Input return Current Rotation
	return GetActorRotation();
}

void ATNHCharacter::SetHealth(float _Health)
{
	Health = _Health;

	if (Health < 0.f)
		Health = 0.f;

	OnHpChanged.Broadcast();
}

void ATNHCharacter::SetMaxHealth(float _MaxHealth)
{
	MaxHealth = _MaxHealth;

	if (MaxHealth < 0.f)
		MaxHealth = 0.f;

	OnHpChanged.Broadcast();
}

void ATNHCharacter::StartLockOn()
{
	if (!IsPlayingExecution)
	{
		if (false == IsLockOnIngaged)
		{
			TargetActor = nullptr;

			EngageLockOn();
			//SetUpdateLockOnTargets(false);
			bIsGuardable = true;

			OnExecutionTargetUpdated(nullptr);
		}
		else
		{
			DisEngageLockOn();
			//SetUpdateLockOnTargets(true);
			bIsGuardable = false;

			if (TiredState)
			{
				SetDesiredGait(EALSGait::Walking);
			}
		}
	}
}

void ATNHCharacter::UpdateLockOnTargets()
{
	FVector CharacterLocation = GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Emplace(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> RemoveTargets;

	for (AActor* Target : LockOnTargets)
	{
		ABaseCharacterClass* BaseCharacter = Cast<ABaseCharacterClass>(Target);

		if (BaseCharacter)
		{
			FVector TargetLocation = BaseCharacter->GetActorLocation();

			float TempDist = FVector::Distance(GetActorLocation(), TargetLocation);

			if (TempDist > SearchDistance)
			{
				RemoveTargets.AddUnique(BaseCharacter);
			}
		}
	}

	for (AActor* Target : RemoveTargets)
	{
		LockOnTargets.Remove(Target);
	}

	TArray<FHitResult> HitResults;

	//if (UKismetSystemLibrary::SphereTraceMultiForObjects(this, CharacterLocation, CharacterLocation, SearchDistance, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResults, true))

	AALSPlayerCameraManager* CameraManager = Cast<AALSPlayerCameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get());
	FVector CameraLocation;
	FRotator CameraRotation;

	if (CameraManager)
	{
		CameraLocation = CameraManager->GetCameraLocation();
		CameraRotation = CameraManager->GetCameraRotation();
	}

	FVector End = CameraLocation + (UKismetMathLibrary::GetForwardVector(CameraRotation) * SearchDistance);

	FVector2D ScreenSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ScreenSize);

		if (UKismetSystemLibrary::BoxTraceMultiForObjects(this, CameraLocation, End,
			FVector(ScreenSize.X * 0.5f, 0.f, ScreenSize.Y * 0.5f), UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::GetRightVector(CameraRotation))
			, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResults, true))
		{
			for (const FHitResult& HitResult : HitResults)
			{
				if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), ULockOnInterface::StaticClass()))
				{
					LockOnTargets.AddUnique(HitResult.GetActor());
				}
			}
		}
	}
}

void ATNHCharacter::UpdateLockOn()
{
	if (CurrentLockOnType == ELockOnType::HardLockOn)
	{
		if (IsLockOnIngaged == true && IsValid(TargetActor))
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(ResumeTimer))
			{
				GetWorld()->GetTimerManager().ClearTimer(ResumeTimer);
			}

			AALSPlayerCameraManager* CameraManager = Cast<AALSPlayerCameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get());
			ABaseCharacterClass* _Target = Cast<ABaseCharacterClass>(TargetActor);
			

			if (CameraManager && _Target)
			{
				FVector CameraLocation = CameraManager->GetCameraLocation();
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%02f %02f %02f"), CameraLocation.X, CameraLocation.Y, CameraLocation.Z));

				FRotator CameraRot = CameraManager->GetCameraRotation();
				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CameraLocation, _Target->LockOnPoint->GetComponentLocation());
				FRotator LookAtRot2 = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());

				CameraRot.Yaw = LookAtRot.Yaw;
				CameraRot.Pitch = LookAtRot.Pitch;

				_Target->TargetActor = this;
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%02f %02f %02f"), CameraRot.Yaw, CameraRot.Pitch, CameraRot.Roll));

				GetWorld()->GetFirstPlayerController()->SetControlRotation(LookAtRot);
				//GetWorld()->GetFirstPlayerController()->SetControlRotation(LookAtRot);
				//
				//SetActorRotation(LookAtRot2);

				CameraManager->SetSwapCameraRotation(LookAtRot);
				//CameraManager->SetCameraRotation(CameraRot);
			}
		}
		else if (IsLockOnIngaged == true && !TargetActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("asdf")));
		}
	}
	else if (CurrentLockOnType == ELockOnType::SoftLockOn)
	{
		if (IsValid(TargetActor))
		{
		}
	}
}

void ATNHCharacter::EngageLockOn()
{
	ResumeLockOn = false;

	UpdateLockOnTargets();

	float LowestDistance = 10000.f;

	for (AActor* Target : LockOnTargets)
	{
		ABaseCharacterClass* NewTarget = Cast<ABaseCharacterClass>(Target);
		FVector TargetLocation = Target->GetActorLocation();

		float TempDist = FVector::Distance(GetActorLocation(), TargetLocation);

		if (IsTargetAble(NewTarget))
		{
			if (TempDist < LowestDistance)
			{
				LowestDistance = TempDist;
				TargetActor = Target;
			}
		}
	}

	if (IsValid(TargetActor))
	{
		IsLockOnIngaged = true;

		CurrentLockOnType = ELockOnType::HardLockOn;

		SetHeadRotation(true);

		SetRotationMode(EALSRotationMode::LookingDirection);

		SetDisableTurnInPlace(false);

		GetWorld()->GetFirstPlayerController()->SetIgnoreLookInput(IsLockOnIngaged);

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		AALSPlayerCameraManager* CameraManager = Cast<AALSPlayerCameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get());

		if (CameraManager)
		{
			CameraManager->SetSwapCamera(true);
		}

		ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(TargetActor);

		if (Target)
		{
			if (Target->OnDeaded.Contains(this, FName(TEXT("OnTargetDead_Func"))) == false)
			{
				Target->OnDeaded.AddDynamic(this, &ThisClass::OnTargetDead_Func);
			}
			if (Target->OnAttackBlocked.Contains(this, FName(TEXT("OnAttackBlocked_Func"))) == false)
			{
				Target->OnAttackBlocked.AddDynamic(this, &ThisClass::OnAttackBlocked_Func);
			}

			SetDesiredGait(EALSGait::Walking);

			SetLockedCollisionRadius(this, true);
			SetLockedCollisionRadius(Target, true);

			OnTargetFinded.Broadcast(TargetActor);
		}

		if (IsNonCombatMode)
		{
			if (SwordEquipAnim)
			{
				GetCharacterAnimInstance()->Montage_Play(SwordEquipAnim);
			}
		}

		OnLockOnState(true, TargetActor, true, true);

		GetWorldTimerManager().SetTimer(
			BreakLockTimer,
			this,
			&ThisClass::BreakLockOn,
			0.15f,
			true
		);
	}
}

void ATNHCharacter::DisEngageLockOn()
{
	if (IsValid(TargetActor))
	{
		ATNHCharacter* Target = Cast<ATNHCharacter>(TargetActor);
		if (Target)
		{
			//Target->SetVisibilityHPBar(false);
		}
	}

	AALSPlayerCameraManager* CameraManager = Cast<AALSPlayerCameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager.Get());

	if (CameraManager)
	{
		CameraManager->SetSwapCamera(false);
	}

	LockOnTargets.Empty();

	ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(TargetActor);

	ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(Ownnigcontroller->GetHUD());
	GameMainHUD->RemoveCustomWidgetFromWidgetPool(Target);
	GameMainHUD->RemoveCustomWidgetFromWidgetPool(this);
	if (Target)
	{
		Target->OnDeaded.RemoveDynamic(this, &ThisClass::OnTargetDead_Func);
		Target->OnAttackBlocked.RemoveDynamic(this, &ThisClass::OnAttackBlocked_Func);

		SetDesiredGait(EALSGait::Running);
		SetLockedCollisionRadius(this, false);
		SetLockedCollisionRadius(Target, false);
	}

	

	IsLockOnIngaged = false;
	ResumeNonCombatMode();
	CurrentLockOnType = ELockOnType::SoftLockOn;

	SetHeadRotation(true);

	SetRotationMode(EALSRotationMode::LookingDirection);

	if (GetDisableTurnInPlace())
	{
		SetDisableTurnInPlace(false);
	}

	GameMainHUD->RemoveAllCustomWidgetFromWidgetPool();

	UCustomWidget_PlayerStateBarBase* PlayerStateBar = Cast< UCustomWidget_PlayerStateBarBase>(GameMainHUD->GetWidgetInHud("PlayerStateBar"));
	OnLockOnState(false, TargetActor, true, true);
	PlayerStateBar->OnPlayerLockOnReleased(TargetActor);

	OnTargetLosted.Broadcast(TargetActor);
	TargetActor = nullptr;

	GetWorld()->GetFirstPlayerController()->ResetIgnoreLookInput();

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetWorldTimerManager().ClearTimer(BreakLockTimer);
}

void ATNHCharacter::BreakLockOn()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("+0.15f"));
	CurrentTimer += 0.15f;
	if (IsValid(TargetActor))
	{
		FVector TargetLocation = TargetActor->GetActorLocation();

		float TempDist = FVector::Distance(GetActorLocation(), TargetLocation);

		if (TempDist >= SearchDistance)
		{
			DisEngageLockOn();
		}
		else if(CurrentTimer >= 1.5f)
		{
			if (IsLockOnIngaged && TargetActor != nullptr)
			{
				auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();
				Commander->SetTargetOffsetPosition(this, TeamNum);
			}

			CurrentTimer = 0;
		}
	}
}

void ATNHCharacter::SetUpdateLockOnTargets(bool _Active)
{
	if (_Active)
	{
		GetWorldTimerManager().SetTimer(
			LockOnTargetUpdateTimer,
			this,
			&ThisClass::UpdateLockOnTargets,
			FindTargetRate,
			true
		);
	}
	else if (_Active == false)
	{
		GetWorldTimerManager().ClearTimer(LockOnTargetUpdateTimer);
	}
}

void ATNHCharacter::SwitchLockedTarget(bool _FindLeftTarget)
{
	UpdateLockOnTargets();

	if (IsLockOnIngaged == true && IsValid(TargetActor))
	{
		if (LockOnTargets.Num() >= 2)
		{
			bool CanSwitch = false;

			AActor* StoredLockOnTarget = TargetActor;
			float SwitchDistance = 100000.f;

			LockOnTargets.Remove(TargetActor);

			FVector MyLocation = GetActorLocation();
			FVector MyRightVector = GetActorRightVector();

			for (AActor* Target : LockOnTargets)
			{
				ABaseCharacterClass* CastTarget = Cast<ABaseCharacterClass>(Target);

				if (IsTargetAble(CastTarget))
				{
					FVector TargetLocation = Target->GetActorLocation();

					FVector Direction = MyLocation - TargetLocation;
					Direction.Normalize();

					double DotValue = UKismetMathLibrary::Dot_VectorVector(MyRightVector, Direction);

					//if less than zero, enemy is exist left of the player
					if (true == _FindLeftTarget)
					{
						if (DotValue >= 0.f)
						{
							float TempDist = FVector::Distance(MyLocation, TargetLocation);

							if (TempDist <= SwitchDistance)
							{
								SwitchDistance = TempDist;
								TargetActor = Target;

								CanSwitch = true;
							}
						}
					}
					else
					{
						if (DotValue < 0.f)
						{
							float TempDist = FVector::Distance(MyLocation, TargetLocation);

							if (TempDist <= SwitchDistance)
							{
								SwitchDistance = TempDist;
								TargetActor = Target;

								CanSwitch = true;
							}
						}
					}
				}
			}

			ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(Ownnigcontroller->GetHUD());

			if (true == CanSwitch)
			{
				if (IsValid(StoredLockOnTarget))
				{
					ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(StoredLockOnTarget);
					if (Target)
					{
						Target->OnDeaded.RemoveDynamic(this, &ThisClass::OnTargetDead_Func);
						Target->OnAttackBlocked.RemoveDynamic(this, &ThisClass::OnAttackBlocked_Func);

						SetLockedCollisionRadius(Target, false);

						//auto TargetDireciton = Cast<UDirectionIndicator_Widget>(GameMainHUD->FindCustomWidgetFromWidgetPool(StoredLockOnTarget));
						//TargetDireciton->SetWidgetOwnningActor(TargetActor);

						OnLockOnState(false, Target, false, true);

						OnTargetLosted_Func(StoredLockOnTarget);
					}
				}
				LockOnTargets.AddUnique(StoredLockOnTarget);

				if (IsValid(TargetActor))
				{
					ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(TargetActor);
					if (Target)
					{
						if (Target->OnDeaded.Contains(this, FName(TEXT("OnTargetDead_Func"))) == false)
						{
							Target->OnDeaded.AddDynamic(this, &ThisClass::OnTargetDead_Func);
						}

						if (Target->OnAttackBlocked.Contains(this, FName(TEXT("OnAttackBlocked_Func"))) == false)
						{
							Target->OnAttackBlocked.AddDynamic(this, &ThisClass::OnAttackBlocked_Func);
						}

						SetLockedCollisionRadius(Target, true);

						//OnTargetFinded.Broadcast(TargetActor);
						OnTargetFinded_Func(Target);

						OnLockOnState(true, Target, false, true);
					}
				}
			}
		}
	}
}

bool ATNHCharacter::IsTargetAble(ABaseCharacterClass* _NewTarget)
{
	bool Check = IsValid(_NewTarget)
		&& _NewTarget->CurrentState != ECharacterState::Dead
		&& _NewTarget->IsMinion() == false;

	return Check;
}

void ATNHCharacter::StopAllAttackMontage(float _BlendTime)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			GetCharacterAnimInstance()->Montage_Stop(_BlendTime, GetAttackInfoData(i, static_cast<ECombatDirection>(j))->AttackAnimation);
		}
	}
}

void ATNHCharacter::PlayNextAttackAnim()
{
	if (AttackData)
	{
		ResumeNonCombatMode();

		SetLockedCollisionRadius(this, false);

		InitMotionWarpingData(TargetActor);

		FAlphaBlendArgs Args;
		Args.BlendTime = 0.3f;

		FOnMontageEnded OnEnded;
		//OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

		//PlayerStateMachine.ChangeState(ECharacterState::Attack);

		if (AttackData->AttackAnimation)
		{
			if (CurrentParryCounterAnim && GetCharacterAnimInstance()->Montage_IsPlaying(CurrentParryCounterAnim) == true)
			{
				FOnMontageEnded Ended;

				GetCharacterAnimInstance()->Montage_SetEndDelegate(Ended, CurrentParryCounterAnim);
				StopAnimMontage(CurrentParryCounterAnim);
			}

			GetCharacterAnimInstance()->Montage_PlayWithBlendIn(AttackData->AttackAnimation, Args, CombatAnimPlayRate, EMontagePlayReturnType::MontageLength, AttackData->PostComboStartTime);
			GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, AttackData->AttackAnimation);
		}
	}
}

void ATNHCharacter::PlayParryAnim(ECombatDirection _Direction)
{
	UAnimMontage* ParryAnim = CharacterAnimationData->Parry[static_cast<uint32>(_Direction)];

	if (ParryAnim && DisableDodge == false)
	{
		FAlphaBlendArgs Args;
		Args.BlendTime = 0.3f;

		//FOnMontageEnded OnMontageEnd;
		//OnMontageEnd.BindUObject(this, &ATNHCharacter::ResetCombat);

		//GetCharacterAnimInstance()->Montage_SetEndDelegate(OnMontageEnd, ParryAnim);
		GetCharacterAnimInstance()->Montage_Play(ParryAnim);
	}
}

void ATNHCharacter::PlayParryCounterAnim(ECombatDirection _Direction, EReactionType _ReactionType)
{
	CurrentParryCounterAnim = CharacterAnimationData->ParryCounter[static_cast<uint32>(_Direction)];

	if (CurrentParryCounterAnim)
	{
		FOnMontageEnded OnEnded;
		OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

		if (CurrentParryCounterAnim
			&& GetCharacterAnimInstance()->Montage_IsPlaying(CurrentParryCounterAnim))
		{
			GetCharacterAnimInstance()->Montage_Stop(0.1f, CurrentParryCounterAnim);
		}

		GetCharacterAnimInstance()->Montage_Play(CurrentParryCounterAnim);
		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, CurrentParryCounterAnim);
	}
}

void ATNHCharacter::StartCombo()
{
	if (CurrentState != ECharacterState::Idle) return;

	CurrentComboIndex = 0;
	StoredNextAttack = EAttack::None;

	if (IsNonCombatMode == false)
	{
		InitNextAttackData(CurrentComboIndex, StoredNextAttackDirection, GetTargetExist());
		PlayNextAttackAnim();
	}
	else
	{
		MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Target"), GetActorLocation(), GetActorRotation());
		AttackReaction = EReactionType::ERT_LowDamage;
		AttackDirection = ECombatDirection::Left;
		DamageAmount = 20.f;
		BalanceAmount = 10.f;
		BlockBalanceAmount = 30.f;
		ParryBalanceAmount = 30.f;

		ResumeNonCombatMode();
		SetOverlayState(EALSOverlayState::SwordTwoHanded);

		FAlphaBlendArgs Args;
		Args.BlendTime = 0.3f;

		FOnMontageEnded OnEnded;
		OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

		if (NonCombatFirstAttack)
		{
			GetCharacterAnimInstance()->Montage_PlayWithBlendIn(NonCombatFirstAttack, Args, CombatAnimPlayRate);
			GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, NonCombatFirstAttack);

			CurrentComboIndex--;
		}
	}
}

void ATNHCharacter::PlayDodgeAnim(int _Direction)
{
	UAnimMontage* DodgeAnim = nullptr;

	if (DisableDodge) return;

	if (TargetActor == nullptr)
	{
		if (CharacterAnimationData->Roll)
		{
			RollDodge = true;
			DodgeAnim = CharacterAnimationData->Roll;
		}
	}
	else
	{
		switch (_Direction)
		{
		case 0: //Back
			if (CharacterAnimationData->Roll)
			{
				RollDodge = true;
				DodgeAnim = CharacterAnimationData->Roll;
			}
			break;
		case 1: //Right
			if (CharacterAnimationData->Dodge[1]) DodgeAnim = CharacterAnimationData->Dodge[1];
			break;
		case 2: //Left
			if (CharacterAnimationData->Dodge[0]) DodgeAnim = CharacterAnimationData->Dodge[0];
			break;
		}
	}

	if (DodgeAnim)
	{
		ResumeNonCombatMode();

		FOnMontageEnded OnEnded;
		OnEnded.BindUObject(this, &ThisClass::ResetCombat);

		StopAllAttackMontage(0.1f);

		GetCharacterAnimInstance()->Montage_Play(DodgeAnim, CombatAnimPlayRate);

		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, DodgeAnim);

		ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(Ownnigcontroller->GetHUD());
		UCustomWidget_PlayerStateBarBase* PlayerStateBar = Cast< UCustomWidget_PlayerStateBarBase>(GameMainHUD->GetWidgetInHud("PlayerStateBar"));
	}
	else
	{
		RollDodge = false;
	}
}

void ATNHCharacter::OnWaitingNextMove()
{
	switch (StoredNextAttack)
	{
	case EAttack::NormalAttack:
	case EAttack::CrushAttack:
		PlayNextAttackAnim();
		StoredNextAttack = EAttack::None;
		break;
	}
}

void ATNHCharacter::CancleAnimation(bool _bReverse)
{
	if (AttackData)
	{
		if (_bReverse)
		{
			float CurrentPlayRate = GetCharacterAnimInstance()->Montage_GetPlayRate(AttackData->AttackAnimation);

			FAlphaBlendArgs Args;
			Args.BlendTime = 0.3f;

			GetCharacterAnimInstance()->Montage_Pause(AttackData->AttackAnimation);
			GetCharacterAnimInstance()->Montage_SetPlayRate(AttackData->AttackAnimation, CurrentPlayRate * -1.f * CombatAnimPlayRate);
			GetCharacterAnimInstance()->Montage_Resume(AttackData->AttackAnimation);
		}
		else
		{
			GetCharacterAnimInstance()->Montage_Stop(0.3f, AttackData->AttackAnimation);
		}
	}
}

void ATNHCharacter::PlayGuardLoopAnim(UAnimMontage* _Montage, bool _bInterrupt)
{
	OnEndedGuardFuncSetted = false;

	GetCharacterAnimInstance()->Montage_Play(CharacterAnimationData->Guard);
	//GetCharacterAnimInstance()->Montage_JumpToSection(FName("Loop"));
}

void ATNHCharacter::ResetCombat(UAnimMontage* _Montage, bool _bInterrupt)
{
	if (_bInterrupt == false)
	{
		PlayerStateMachine.ChangeState(ECharacterState::Idle);

		BeforeExecutionData = nullptr;
		IsPlayingExecution = false;

		EliteExecution = false;

		RollDodge = false;

		DoOnce.Reset();

		//SetLockedCollisionRadius(this, IsLockOnIngaged);
	}
}

void ATNHCharacter::ForceResetCombat()
{
	PlayerStateMachine.ChangeState(ECharacterState::Idle);

	BeforeExecutionData = nullptr;
}

void ATNHCharacter::OnHited_Func()
{
	ForceResetCombat();

	if (GetHealthRatio() <= 0.5f)
	{
		if (AlarmOnce_LowHP.Execute())
		{
			OnLowHP.Broadcast();
		}
	}
	else
	{
		if (AlarmOnce_EnoughHP.Execute())
		{
			OnEnoughHP.Broadcast();
		}
	}
}

void ATNHCharacter::OnLowHP_Func()
{
	AlarmOnce_EnoughHP.Reset();

	SetOverlayState(EALSOverlayState::Injured);
}

void ATNHCharacter::OnEnoughHP_Func()
{
	AlarmOnce_LowHP.Reset();

	SetOverlayState(EALSOverlayState::SwordOneHanded);
}

void ATNHCharacter::OnHPChanged_Func()
{
	if (GetHealthRatio() <= 0.5f)
	{
		if (AlarmOnce_LowHP.Execute())
		{
			OnLowHP.Broadcast();
		}
	}
	else
	{
		if (AlarmOnce_EnoughHP.Execute())
		{
			OnEnoughHP.Broadcast();
		}
	}
}

void ATNHCharacter::OnTargetDead_Func()
{
	if (IsLockOnIngaged)
	{
		DisEngageLockOn();
		//ResumeLockOn = true;
	}

	if (ResumeLockOn)
	{
		EngageLockOn();

		//FTimerHandle LockOnCoolTime;
		//GetWorld()->GetTimerManager().SetTimer(LockOnCoolTime, FTimerDelegate::CreateLambda([&]()
		//	{

		//		GetWorld()->GetTimerManager().ClearTimer(LockOnCoolTime);
		//	}), 0.5f, false);
	}
}

void ATNHCharacter::OnTargetFinded_Func(AActor* _Target)
{
	ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(_Target);

	if (Target)
	{
		Target->IsPlayerTarget = true;
	}
}

void ATNHCharacter::OnTargetLosted_Func(AActor* _Target)
{
	ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(Ownnigcontroller->GetHUD());

	AAi_CharacterBase* TargetAi = Cast<AAi_CharacterBase>(_Target);

	if (TargetAi)
	{
		TargetAi->IsPlayerTarget = false;
	}
}

void ATNHCharacter::CheckNextAttack()
{
	IsContinueAttack = false;
	IsContinueExecution = false;
	IsCheckingKey = true;

	StoredNextAttack = EAttack::None;
}

void ATNHCharacter::OnAttackBlocked_Func()
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(HitCameraShake);
}

void ATNHCharacter::UpdateHealthRegen(float _Delta)
{
	if (bHealthRegen && Health < MaxHealth)
	{
		Health = FMath::Clamp(Health + CharacterInfo->RV_Health * _Delta, MinHealth, MaxHealth);

		ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
		mainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);
	}
}

void ATNHCharacter::ContinueAttack()
{
	if (IsContinueExecution)
	{
		if (GetGait() != EALSGait::Sprinting)
		{
			//FindExecutionTarget();

			if (IsLockOnIngaged == false && Execution_Target)
			{
				CurrentExecution_Target = Execution_Target;

				DoOnce.Reset();

				Execution_Attacker(-1);
			}
		}
	}
	else if (IsContinueAttack)
	{
		switch (StoredNextAttack)
		{
		case EAttack::NormalAttack:
			InitNextAttackData(++CurrentComboIndex, StoredNextAttackDirection, GetTargetExist());
			break;
		}

		PlayNextAttackAnim();
	}
}

void ATNHCharacter::Attack(bool _Down)
{
	LButtonDown = _Down;
	StoredNextAttackDirection = CurrentDirection;

	if (_Down == true)//OnDown
	{
		switch (CurrentState)
		{
		case ECharacterState::Idle:
		{
			if (IsPlayingExecution == false)
			{
				if (IsLockOnIngaged == false && Execution_Target)
				{
					CurrentExecution_Target = Execution_Target;

					Execution_Attacker(-1);
					break;
				}
				else if (IsLockOnIngaged == true)
				{
					ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(TargetActor);

					if (Target && Target->CurrentState == ECharacterState::Stun)
					{
						CurrentExecution_Target = Target;

						if (IsLockOnIngaged)
						{
							DisEngageLockOn();
						}
						Execution_Attacker(-1);
						break;
					}
				}

				StartCombo();
			}
		}
		break;
		case ECharacterState::Attack:
		{
			if (IsCheckingKey)
			{
				IsContinueAttack = true;
				StoredNextAttack = EAttack::NormalAttack;
			}
		}
		break;
		case ECharacterState::Guard:
		{
			//if (CanUseParrying())
			//{
			//	PlayerStateMachine.ChangeState(ECharacterState::Parrying);
			//}
		}
		break;
		case ECharacterState::Execution:
		{
			if (IsCheckingKey)
			{
				IsContinueExecution = true;
			}
		}
		break;
		}
	}
	else if (_Down == false)//OnReleased
	{
	}
}

void ATNHCharacter::RecoverdBalace()
{
	Super::RecoverdBalace();

	if (balance >= Maxbalance)
	{
		SetTiredState(false);
		bIsRecoverdBalance = false;
	}

	if (StateBar != nullptr)
	{
		ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
		mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
	}
}

void ATNHCharacter::DoParring(ECombatDirection _Direction, EReactionType _ReactionType)
{
	Super::DoParring(_Direction, _ReactionType);

	bParryingSuccess = true;

	PlayParryCounterAnim(_Direction, _ReactionType);

	PlayerStateMachine.ChangeState(ECharacterState::Attack);
}

void ATNHCharacter::OnSwordHit(FHitResult _LastItem)
{
	ABaseCharacterClass* HitPawn = Cast<ABaseCharacterClass>(_LastItem.GetActor());
	if (HitPawn == nullptr || HitPawn->CurrentState == ECharacterState::Dead) return;

	float Damage = DamageAmount;
	if (HitPawn->IsMinion()) Damage = 10000.f;

	if (IsPlayingExecution)
	{
		ABaseCharacterClass* Target = Cast<ABaseCharacterClass>(CurrentExecution_Target);
		if (Target == HitPawn)
		{
			UGameplayStatics::ApplyPointDamage(HitPawn, Damage, _LastItem.ImpactNormal, _LastItem, GetController(), this, nullptr);
		}
	}
	else if (HitPawn != nullptr)
	{
		UGameplayStatics::ApplyPointDamage(HitPawn, Damage, _LastItem.ImpactNormal, _LastItem, GetController(), this, nullptr);
	}
}

void ATNHCharacter::UpdateDirectionIndicator()
{
	FVector2D MouseCurrentPos;
	float MouseDeltaX, MouseDeltaY;
	GetWorld()->GetFirstPlayerController()->GetInputMouseDelta(MouseDeltaX, MouseDeltaY);
	GetWorld()->GetFirstPlayerController()->GetMousePosition(MouseCurrentPos.X, MouseCurrentPos.Y);

	float MoveDist = FVector2D::Distance(MouseBeforePos, MouseCurrentPos);

	//float MovementMouseX = UKismetMathLibrary::MapRangeClamped(FMath::Abs(MouseX), DirectionSensitivity, 2.0f, 0.f, 1.0f);

	if (MoveDist > DirectionSensitivity)
	{
		if (MouseDeltaX > 0.0f)
		{
			CurrentDirection = ECombatDirection::Right;
		}
		else if (MouseDeltaX < 0.0f)
		{
			CurrentDirection = ECombatDirection::Left;
		}

		MouseBeforePos = MouseCurrentPos;
	}
}

void ATNHCharacter::Parry(bool _Down)
{
	//RButtonDown = _Down;

	if (_Down && TargetActor != nullptr && DisableDodge == false)
	{
		if (CurrentState == ECharacterState::Idle || CurrentState == ECharacterState::GuardHit)
		{
			PlayParryAnim(CurrentDirection);
		}
	}
}

void ATNHCharacter::ApplyDamageBalance(float Damage)
{
	if (balance - Damage >= 0)
	{
		balance -= Damage;

		if (TiredState == false)
		{
			RecoveredTime = CharacterInfo->RT_Balance;
		}
	}
	else
	{
		balance = 0;

		if (TiredState == false)
		{
			RecoveredTime = CharacterInfo->RT_Balance;
		}
	}

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);

	if (balance == 0.f)
	{
		SetTiredState(true);
		bIsRecoverdBalance = true;
	}
}

void ATNHCharacter::Dodge()
{
	if (DisableDodge
		|| CurrentState == ECharacterState::Execution
		|| CurrentState == ECharacterState::GuardHit
		|| TiredState == true
		|| balance <= 0.f) return;

	DodgeCost = 0;
	int Direction = -1;

	if ((ForwardScale == 0.f && RightScale == 0.f)
		|| TargetActor == nullptr)
	{
		Direction = 0;
		DodgeCost = NonCombatRollDodgeAmount;
	}
	else
	{
		if (ForwardScale < 0)
		{
			Direction = 0;
			DodgeCost = RollDodgeAmount;//DTETC->BackRollCost
		}
		else if (RightScale > 0)
		{
			Direction = 1;
			DodgeCost = SideDodgeAmount;//DTETC->DodgeCost
		}
		else if (RightScale < 0)
		{
			Direction = 2;
			DodgeCost = SideDodgeAmount;//DTETC->DodgeCost
		}
	}

	BeforeDodgeLocation = GetActorLocation();
	PlayDodgeAnim(Direction);
}

void ATNHCharacter::Crash(bool _Down)
{
	WheelDown = _Down;

	if (_Down)
	{
		if (CurrentState == ECharacterState::Idle)
		{
			PlayerStateMachine.ChangeState(ECharacterState::Attack);
			InitNextAttackData(-1, ECombatDirection::Left);
			PlayNextAttackAnim();
		}
		else if (CurrentState == ECharacterState::Attack)
		{
			if (IsCheckingKey)
			{
				IsContinueAttack = true;
				StoredNextAttack = EAttack::CrushAttack;
			}
		}
	}
}

void ATNHCharacter::Ability(bool _Down)
{
	if (_Down)
	{
		if (UKismetSystemLibrary::IsStandalone(this))
		{
			UGameplayStatics::SetGlobalTimeDilation(this, 0.1f);
		}
	}
	else
	{
		if (UKismetSystemLibrary::IsStandalone(this))
		{
			UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
		}
	}
}

bool ATNHCharacter::CheckExecution()
{
	FindExecutionTarget();

	if (Execution_Target)
	{
		Execution_Attacker(1);
	}

	return (Execution_Target != nullptr);
}

void ATNHCharacter::Cancle()
{
	if (bCancleAnimation)
	{
		CancleAnimation(true);
	}
}

void ATNHCharacter::ResumeNonCombatMode()
{
	GetWorld()->GetTimerManager().ClearTimer(ResumeTimer);

	TWeakObjectPtr<ATNHCharacter> WeakPtr = this;

	GetWorld()->GetTimerManager().SetTimer(ResumeTimer, FTimerDelegate::CreateLambda([WeakPtr]()
		{
			if (WeakPtr.IsValid())  // 객체 유효성 검사
			{
				if (WeakPtr->IsNonCombatMode == false)
				{
					WeakPtr->GetCharacterAnimInstance()->Montage_Play(WeakPtr->SwordUnEquipAnim);
				}
			}
		}
	), NonCombatResumeTime, false);
}

void ATNHCharacter::InitalizeFSM()
{
	FSM_Idle();
	FSM_Attack();
	FSM_Guard();
	FSM_Dodge();
	FSM_Parry();
	FSM_Weak();
	FSM_Stun();
	FSM_Stagger();
	FSM_GuardHit();

	//StartState
	PlayerStateMachine.ChangeState(ECharacterState::Idle);
}

void ATNHCharacter::FSM_Idle()
{
	CreateStateParameter Idle_Param;

	Idle_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Idle;
		};

	Idle_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Idle")));
		};

	Idle_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Idle, Idle_Param);
}

void ATNHCharacter::FSM_Attack()
{
	CreateStateParameter Attack_Param;

	Attack_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Attack;
		};

	Attack_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Attack_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Attack, Attack_Param);
}

void ATNHCharacter::FSM_Guard()
{
	CreateStateParameter Guard_Param;

	Guard_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Guard;

			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("GuardStart")));
		};

	Guard_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Guard_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Guard, Guard_Param);
}

void ATNHCharacter::FSM_Dodge()
{
	CreateStateParameter Dodge_Param;

	Dodge_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Dodge;
		};

	Dodge_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
			ApplyDamageBalance(DodgeCost * _Delta);
		};

	Dodge_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Dodge, Dodge_Param);
}

void ATNHCharacter::FSM_Parry()
{
	CreateStateParameter Parry_Param;

	Parry_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Parrying;
			ParryCancleOnce.Reset();
		};

	Parry_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Parry_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Parrying, Parry_Param);
}

void ATNHCharacter::FSM_Weak()
{
	CreateStateParameter Weak_Param;

	Weak_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Weak;
		};

	Weak_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Weak_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Weak, Weak_Param);
}

void ATNHCharacter::FSM_Stun()
{
	CreateStateParameter Stun_Param;

	Stun_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Stun;
		};

	Stun_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Stun_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Stun, Stun_Param);
}

void ATNHCharacter::FSM_Stagger()
{
	CreateStateParameter Stagger_Param;

	Stagger_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::Stagger;
		};

	Stagger_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	Stagger_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::Stagger, Stagger_Param);
}

void ATNHCharacter::FSM_GuardHit()
{
	CreateStateParameter GuardHit_Param;

	GuardHit_Param.Start = [&](class StateMachine* _Parent)
		{
			CurrentState = ECharacterState::GuardHit;
		};

	GuardHit_Param.Stay = [&](float _Delta, class StateMachine* _Parent)
		{
		};

	GuardHit_Param.End = [&](class StateMachine* _Parent)
		{
		};

	PlayerStateMachine.CreateState(ECharacterState::GuardHit, GuardHit_Param);
}

void ATNHCharacter::ClearDataAll()
{
	Super::ClearDataAll();

	//ForceResetCombat();
}

void ATNHCharacter::FSM_PlayerChangeState(ECharacterState _State)
{
	PlayerStateMachine.ChangeState(_State);
}

UAnimMontage* ATNHCharacter::LowDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedMontage = Super::LowDamageReaction(_DamageCauser, _Direction, _PlaySecond);

	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

	if (SelectedMontage)
	{
		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, SelectedMontage);
		//GetCharacterAnimInstance()->Montage_Play(SelectedMontage);
	}

	return nullptr;
}

void ATNHCharacter::OnEnded_Attacker(UAnimMontage* _VictimMontage, bool _Interrupt)
{
	GetWorld()->GetFirstPlayerController()->SetIgnoreMoveInput(false);

	//GetMesh()->SetCollisionProfileName(FName("CharacterMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

	FRotator CurrentRotation = GetActorRotation();

	CurrentRotation.Pitch = 0.f;

	SetActorRotation(CurrentRotation);

	ResumeNonCombatMode();

	balance = Maxbalance;

	if (CurrentExecution_Target)
	{
		if (CurrentExecution_Target->IsMinion())
		{
			Health = FMath::Clamp(Health + MinionExecutionHeal_HP, 0, MaxHealth);
		}
		else
		{
			Health = FMath::Clamp(Health + EliteExecutionHeal_HP, 0, MaxHealth);
		}

		if (CurrentExecution_Target->pawnType >= EPawnType::Boss1 && CurrentExecution_Target->pawnType <= EPawnType::Boss2)
		{
			ResumeLockOn = true;
			OnTargetDead_Func();
		}
	}

	SetTiredState(false);

	ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
	mainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);

	OnExecutionEnded();
}

void ATNHCharacter::SetTiredState(bool _bValue)
{
	OnPlayerTired(_bValue);
	if (_bValue)
	{
		TiredState = true;

		MovementModel.RowName = "Tired";
		SetMovementModel();
		ForceUpdateCharacterState();

		CombatAnimPlayRate = AnimPlayRateChange;

		DamagePostProcess->SetVisibility(true);
	}
	else
	{
		TiredState = false;

		if (GetDesiredGait() == EALSGait::Walking && IsLockOnIngaged == false)
		{
			SetDesiredGait(EALSGait::Running);
		}

		MovementModel.RowName = "Normal";
		SetMovementModel();
		ForceUpdateCharacterState();

		CombatAnimPlayRate = 1.f;
		DamagePostProcess->SetVisibility(false);
	}
}

UAnimMontage* ATNHCharacter::LowDamageReaction_Minion(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CharacterAnimationData)
	{
		if (_Direction >= 45 && _Direction <= 135)//Front
		{
			SelectedReaction = CharacterAnimationData->LowDamage_Reaction_Minion[static_cast<int32>(EHitDirection::Front)];
		}
		else if (_Direction >= 135 && _Direction <= 225)//Left
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Left")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction_Minion[static_cast<int32>(EHitDirection::Left)];
		}
		else if (_Direction >= 255 && _Direction <= 315)//Rear
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Rear")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction_Minion[static_cast<int32>(EHitDirection::Rear)];
		}
		else//Right
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Right")));

			SelectedReaction = CharacterAnimationData->LowDamage_Reaction_Minion[static_cast<int32>(EHitDirection::Right)];
		}

		if (SelectedReaction)
		{
			GetCharacterAnimInstance()->Montage_Play(SelectedReaction);
		}
	}

	return SelectedReaction;
}

void ATNHCharacter::InitalizeExecutionData()
{
	if (ExecutionInfoData)
	{
		for (auto RowName : ExecutionInfoData->GetRowNames())
		{
			FString RowNameString = RowName.ToString();
			FExecutionData* ExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*RowNameString, "");

			if (RowNameString.Find("Boss1") != -1)
			{
				BossExecutionMontages.AddUnique(RowNameString);
			}
			else if (RowNameString.Find("Elite") != -1)
			{
				EliteExecutionMontages.AddUnique(RowNameString);
			}
			else if (RowNameString.Find("Minion") != -1)
			{
				if (ExecutionData)
				{
					NormalExecutionMontages.AddUnique(RowNameString);

					if (ExecutionData->Level == 0)
					{
						NormalExecutionStartMontages.AddUnique(RowNameString);
					}
				}
			}

			if (ExecutionData)
			{
				ExecutionDatas.Add(RowNameString, ExecutionData);
			}
		}
	}
}

UAnimMontage* ATNHCharacter::GuardBreakReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedMontage = Super::GuardBreakReaction(_DamageCauser, _Direction);

	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

	if (SelectedMontage)
	{
		//GetCharacterAnimInstance()->Montage_Play(SelectedMontage);
		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, SelectedMontage);
	}

	return nullptr;
}

UAnimMontage* ATNHCharacter::HighDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond)
{
	UAnimMontage* SelectedMontage = Super::HighDamageReaction(_DamageCauser, _Direction);

	FOnMontageEnded OnEnded;
	OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

	if (SelectedMontage)
	{
		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, SelectedMontage);
		//GetCharacterAnimInstance()->Montage_Play(SelectedMontage);
	}

	return nullptr;
}

UAnimMontage* ATNHCharacter::KnockBackReaction(AActor* _DamageCauser, FVector _DirVector)
{
	UAnimMontage* SelectedMontage = Super::KnockBackReaction(_DamageCauser, _DirVector);

	return SelectedMontage;
}

UAnimMontage* ATNHCharacter::GuardReaction(AActor* _DamageCauser, ECombatDirection _Direction)
{
	UAnimMontage* SelectedReaction = nullptr;

	if (CharacterAnimationData)
	{
		int index = FMath::Clamp(static_cast<uint32>(_Direction), 0, 1);

		if (CharacterAnimationData->Guard_Reaction.Num() > index)
		{
			SelectedReaction = CharacterAnimationData->Guard_Reaction[index];

			FOnMontageEnded OnEnded;
			OnEnded.BindUObject(this, &ATNHCharacter::ResetCombat);

			if (SelectedReaction)
			{
				GetCharacterAnimInstance()->Montage_Play(SelectedReaction);
				GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, SelectedReaction);
			}
		}
	}

	return SelectedReaction;
}

UAnimMontage* ATNHCharacter::DeathReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedReaction = Super::DeathReaction(_DamageCauser, _Direction);

	CurrentState = ECharacterState::Dead;

	return nullptr;
}

UAnimMontage* ATNHCharacter::GroggyReaction(AActor* _DamageCauser, int _Direction)
{
	return nullptr;
}

void ATNHCharacter::Execution_Attacker(int32 _index)
{
	if (IsValid(CurrentExecution_Target) 
		&& CurrentExecution_Target->ExecutionAble 
		&& DoOnce.CanExecute()
		&& CanExecution(CurrentExecution_Target))
	{
		FString ExecutionRowName;

		CurrentExecutionData = nullptr;

		bool NonTargetAttack = false;

		if (_index == -1)//RandomSelect
		{
			if (CurrentExecution_Target->pawnType >= EPawnType::NormalSword
				&& CurrentExecution_Target->pawnType <= EPawnType::NormalArcher)
			{
				if (BeforeExecutionData != nullptr)
				{
					if (BeforeExecutionData->NextComboExecutions.IsEmpty() == false)
					{
						int32 RandomValue = UKismetMathLibrary::RandomInteger(BeforeExecutionData->NextComboExecutions.Num());

						ExecutionRowName = BeforeExecutionData->NextComboExecutions[RandomValue];

						CurrentExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*ExecutionRowName, "");
					}

					if (CurrentExecutionData == nullptr) BeforeExecutionData = nullptr;
				}

				if (BeforeExecutionData == nullptr)
				{
					if (IsNonCombatMode)
					{
						FString str = "NonCombat_0";
						CurrentExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*str, "");
					}
					else
					{
						float TargetBetweenDist = FVector::Distance(CurrentExecution_Target->GetActorLocation(), GetActorLocation());
						TArray<FExecutionData*> TempDatas;

						for (FString str : NormalExecutionStartMontages)
						{
							FExecutionData* TempExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*str, "");

							if (TempExecutionData)
							{
								if (TempExecutionData->ExecutionOutRange <= TargetBetweenDist && TargetBetweenDist <= TempExecutionData->ExecutionInRange)
								{
									TempDatas.Add(TempExecutionData);
								}
							}
						}

						if (!TempDatas.IsEmpty())
						{
							int32 NormalCount = TempDatas.Num();
							int32 RandNum = UKismetMathLibrary::RandomInteger(NormalCount);

							CurrentExecutionData = TempDatas[RandNum];
						}
					}
				}
			}
			else if (CurrentExecution_Target->pawnType >= EPawnType::EliteSword
				&& CurrentExecution_Target->pawnType <= EPawnType::HighEliteGladiator)
			{
				if (CurrentExecution_Target->CurrentState == ECharacterState::Stun)
				{
					int32 EliteCount = EliteExecutionMontages.Num();
					ExecutionRowName = "Elite_";
					ExecutionRowName.Append(*FString::FromInt(UKismetMathLibrary::RandomInteger(EliteCount)));

					CurrentExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*ExecutionRowName, "");
				}
				else
				{
					NonTargetAttack = true;
				}
			}
			else if (CurrentExecution_Target->pawnType == EPawnType::Boss1)
			{
				if (CurrentExecution_Target->CurrentState == ECharacterState::Stun)
				{
					int32 BossExecutionCount = BossExecutionMontages.Num();
					ExecutionRowName = "Boss1_";
					ExecutionRowName.Append(*FString::FromInt(UKismetMathLibrary::RandomInteger(BossExecutionCount)));

					CurrentExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*ExecutionRowName, "");
				}
			}
			else if (CurrentExecution_Target->pawnType == EPawnType::Boss2)
			{
				if (CurrentExecution_Target->CurrentState == ECharacterState::Stun)
				{
					ABossSecondPhase* Boss = Cast<ABossSecondPhase>(CurrentExecution_Target);

					ExecutionRowName = "Boss2_";
					ExecutionRowName.Append(*FString::FromInt(Boss->GetExecutionCount()));

					CurrentExecutionData = ExecutionInfoData->FindRow<FExecutionData>(*ExecutionRowName, "");
				}
			}
		}

		if (CurrentExecutionData && NonTargetAttack == false)
		{
			if (CurrentParryCounterAnim && GetCharacterAnimInstance()->Montage_IsPlaying(CurrentParryCounterAnim) == true)
			{
				FOnMontageEnded Ended;

				GetCharacterAnimInstance()->Montage_SetEndDelegate(Ended, CurrentParryCounterAnim);
				StopAnimMontage(CurrentParryCounterAnim);
			}

			DoOnce.Execute();

			OnExecutionTargetUpdated(nullptr);

			CurrentExecution_Target->OnExecuted(this);

			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = 0;

			GetWorld()->GetFirstPlayerController()->SetIgnoreMoveInput(true);

			//GetMesh()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));

			GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

			FOnMontageEnded OnEnded;
			OnEnded.BindUObject(this, &ThisClass::OnEnded_Attacker);

			if (CurrentExecution_Target->OnDeaded.Contains(this, FName(TEXT("OnTargetDead_Func"))) == false)
			{
				CurrentExecution_Target->OnDeaded.AddDynamic(this, &ThisClass::OnTargetDead_Func);
			}

			IsPlayingExecution = true;
			EliteExecution = false;

			GetWorld()->GetTimerManager().ClearTimer(ResumeTimer);

			GetCharacterAnimInstance()->Montage_Play(CurrentExecutionData->Execution_Attacker);

			CurrentState = ECharacterState::Execution;

			GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, CurrentExecutionData->Execution_Attacker);

			FVector NextLocation = CurrentExecution_Target->GetActorLocation() +
				UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(CurrentExecution_Target->GetActorLocation(), GetActorLocation())) * CurrentExecutionData->BetweenDistance;
			FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(NextLocation, CurrentExecution_Target->GetActorLocation());

			if (CurrentExecution_Target->IsMinion() == false
				&& CanExecution(CurrentExecution_Target))
			{
				CurrentExecution_Target->Execution_Victim(this, CurrentExecutionData->Execution_Victim, CurrentExecutionData->BetweenDistance);

				EliteExecution = true;

				OnExecutionStarted(*CurrentExecutionData);
			}
			else
			{
				float Dist = FVector::Distance(CurrentExecution_Target->GetActorLocation(), NextLocation);
				if (Dist > CurrentExecutionData->BetweenDistance)
				{
					NextLocation = GetActorLocation();
				}

				EliteExecution = false;

				AttackReaction = EReactionType::ERT_Execution;
				AttackDirection = ECombatDirection::Crash;
				DamageAmount = 9990000.f;
				BalanceAmount = 10000.f;
				BlockBalanceAmount = 10000.f;
				ParryBalanceAmount = 10000.f;
			}
			CurrentExecution_Target->Execution_Causer = this;
			CurrentExecution_Target->GetController()->StopMovement();

			FVector Size;

			Size.X = (GetCapsuleComponent()->GetScaledCapsuleRadius());
			Size.Y = 10.f;
			Size.Z = (GetCapsuleComponent()->GetScaledCapsuleRadius());

			TArray<FHitResult> BoxHitResults;

			TEnumAsByte<ETraceTypeQuery> TraceType = UEngineTypes::ConvertToTraceType(ECC_WorldStatic);

			TArray<AActor*> Ignore;
			Ignore.AddUnique(CurrentExecution_Target);

			UKismetSystemLibrary::BoxTraceMulti(this, GetActorLocation(), CurrentExecution_Target->GetActorLocation(), Size, FRotator::ZeroRotator, TraceType,
				false, Ignore, EDrawDebugTrace::None, BoxHitResults, true);

			bool bHit = false;

			for (const FHitResult& HitRes : BoxHitResults)
			{
				ABaseCharacterClass* TempActor = Cast<ABaseCharacterClass>(HitRes.GetActor());

				if (TempActor && TempActor->IsMinion())
				{
					TempActor->OnPushed();
				}
			}

			UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), NextLocation, NextRotation, false, false, ExecutionMoveTime, false,
				EMoveComponentAction::Move, LatentInfo);

			BeforeExecutionData = CurrentExecutionData;
		}
		else if (NonTargetAttack == true)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = 0;

			FVector NextLocation = CurrentExecution_Target->GetActorLocation() +
				UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(CurrentExecution_Target->GetActorLocation(), GetActorLocation())) * 150.f;
			FRotator NextRotation = UKismetMathLibrary::FindLookAtRotation(NextLocation, CurrentExecution_Target->GetActorLocation());

			if (CurrentExecution_Target->IsMinion() == false)
			{
				float Dist = FVector::Distance(CurrentExecution_Target->GetActorLocation(), NextLocation);
				if (Dist > 150.f)
				{
					NextLocation = GetActorLocation();
				}
			}

			UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), NextLocation, NextRotation, false, false, ExecutionMoveTime, false,
				EMoveComponentAction::Move, LatentInfo);

			StartCombo();
		}
	}
}

float ATNHCharacter::GetHealthRatio()
{
	float Ratio = (Health / MaxHealth);

	return Ratio;
}