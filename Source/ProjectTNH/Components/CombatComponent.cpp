// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectTNH/Interfaces/LockOnInterface.h"
#include "ProjectTNH/Interfaces/InteractableInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "TimerManager.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "ProjectTNH/Weapon/WeaponTypes.h"
#include "DidItHitActorComponent.h"
#include "ProjectTNH/UI/HPBar_Widget.h"
#include "Components/WidgetComponent.h"
#include "ProjectTNH/ProjectTNH.h"
#include "ProjectTNH/UI/DirectionIndicator_Widget.h"
#include "ProjectTNH/DamageType/DT_LongSwordAttack.h"
#include "ProjectTNH/Character/CharacterState.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IsValid(Character))
	{

	}

	if (Character && SpawnWeaponClass)
	{
		FActorSpawnParameters Params;

		Params.Instigator = Character;
		Params.Owner = Character;

		AWeaponBase* TempWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnWeaponClass, Character->GetActorTransform(), Params);
		TempWeapon->SetEquipSocket(Character->GetCombatComponent()->GetWeaponSocket(TempWeapon, true));
		TempWeapon->SetUnEquipSocket(Character->GetCombatComponent()->GetWeaponSocket(TempWeapon, false));
		TempWeapon->OnInit();

		Character->GetCombatComponent()->SetMainWeapon(TempWeapon);

		TempWeapon->OnEquipped();
		InitOnWeaponEquip();
		SetWeaponEquiped(true);
	}

	//Character->GetWorldTimerManager().SetTimer(SoftLockOnUpdateTimer, this, &UCombatComponent::SoftLockOnUpdate, 0.1f, true);
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	UpdateCharge(DeltaTime);
	
	CheckLockOn();
	//DetectAttackDirection();
}

void UCombatComponent::EngageLockOn()
{
	if (Character)
	{
		FVector CharacterLocation = Character->GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
		ObjectType.Emplace(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<FHitResult> HitResults;

		if (UKismetSystemLibrary::SphereTraceMultiForObjects(this, CharacterLocation, CharacterLocation, SearchDistance, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResults, true))
		{
			for (const FHitResult& HitResult : HitResults)
			{
				if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), ULockOnInterface::StaticClass()))
				{
					float LowestDistance = 10000.f;

					LockOnTargets.AddUnique(HitResult.GetActor());

					for (AActor* Target : LockOnTargets)
					{
						FVector MyLocation = Character->GetActorLocation();
						FVector TargetLocation = Target->GetActorLocation();

						float TempDist = FVector::Distance(MyLocation, TargetLocation);

						if (TempDist < LowestDistance)
						{
							LowestDistance = TempDist;
							TargetActor = Target;
						}
					}
				}
			}
		}
	}
}

void UCombatComponent::CheckLockOn()
{
	if (Character)
	{
		//if (LockOnMode == ELockOnType::ELT_HardLockOn && IsValid(TargetActor))
		//{
		//	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(),
		//		TargetActor->GetActorLocation());

		//	GetWorld()->GetFirstPlayerController()->SetControlRotation(LookAtRot);
		//}
		//else if (LockOnMode==ELockOnType::ELT_SoftLockOn && IsValid(TargetActor))
		//{
		//	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(),
		//		TargetActor->GetActorLocation());

		//	if (IsDodging == false)
		//	{
		//		FRotator CurrentActorRotation = Character->GetActorRotation();

		//		if (false == CurrentActorRotation.Equals(LookAtRot))
		//		{
		//			float RotationSpeed = 15.f;
		//			FRotator NewActorRotation;

		//			NewActorRotation = UKismetMathLibrary::RLerp(CurrentActorRotation, LookAtRot, RotationSpeed * UGameplayStatics::GetWorldDeltaSeconds(Character), true);

		//			Character->SetActorRotation(NewActorRotation);
		//		}
		//	}
		//}
	}
}

void UCombatComponent::LockOnAction()
{
	if (Character)
	{
		if (false == IsLockOnIngaged)
		{
			EngageLockOn();
			HardLockOn();
		}
		else
		{
			DisEngageLockOn();
		}
	}
}

void UCombatComponent::DisEngageLockOn()
{
	if (Character)
	{
		if (IsValid(TargetActor))
		{
			ATNHCharacter* Target = Cast<ATNHCharacter>(TargetActor);
			if (Target)
			{
				Target->GetCombatComponent()->SetVisibilityHPBar(false);
			}
		}

		LockOnTargets.Empty();

		TargetActor = nullptr;

		IsLockOnIngaged = false;

//		LockOnMode = ELockOnType::ELT_None;

		Character->SetHeadRotation(true);

		Character->SetRotationMode(EALSRotationMode::LookingDirection);

		if (Character->GetDisableTurnInPlace())
		{
			Character->SetDisableTurnInPlace(false);
		}

		GetWorld()->GetFirstPlayerController()->ResetIgnoreLookInput();

		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;

		Character->GetWorldTimerManager().ClearTimer(BreakLockTimer);
	}
}

void UCombatComponent::BreakLockOn()
{
	if (IsValid(TargetActor))
	{
		FVector MyLocation = Character->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();

		float TempDist = FVector::Distance(MyLocation, TargetLocation);

		if (TempDist >= SearchDistance)
		{
			DisEngageLockOn();
		}
	}
}

void UCombatComponent::HardLockOn()
{
	if (IsValid(TargetActor))
	{
		IsLockOnIngaged = true;

//		LockOnMode = ELockOnType::ELT_HardLockOn;

		Character->SetHeadRotation(true);

		Character->SetRotationMode(EALSRotationMode::LookingDirection);

		Character->SetDisableTurnInPlace(false);

		GetWorld()->GetFirstPlayerController()->SetIgnoreLookInput(IsLockOnIngaged);

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;

		ATNHCharacter* Target = Cast<ATNHCharacter>(TargetActor);
		if (Target)
		{
			Target->GetCombatComponent()->SetVisibilityHPBar(true);
		}

		Character->GetWorldTimerManager().SetTimer(
			BreakLockTimer,
			this,
			&UCombatComponent::BreakLockOn,
			0.15f,
			true
		);
	}
}

void UCombatComponent::SoftLockOn()
{
	if (IsValid(TargetActor))
	{
//		LockOnMode = ELockOnType::ELT_SoftLockOn;

		Character->SetHeadRotation(false);

		Character->SetRotationMode(EALSRotationMode::VelocityDirection);

		Character->SetDisableTurnInPlace(true);

		Character->GetWorldTimerManager().SetTimer(
			BreakLockTimer,
			this,
			&UCombatComponent::BreakLockOn,
			0.15f,
			true
		);
	}
}

void UCombatComponent::SetFocusEnemy(AActor* _Enemy)
{
	if (IsValid(_Enemy))
	{
		ATNHCharacter* oldTarget = Cast<ATNHCharacter>(TargetActor);
		ATNHCharacter* newTarget = Cast<ATNHCharacter>(_Enemy);

		if (oldTarget)
		{
			oldTarget->GetCombatComponent()->SetVisibilityHPBar(false);
		}
		
		if(newTarget)
		{
			TargetActor = _Enemy;

			newTarget->GetCombatComponent()->SetVisibilityHPBar(true);
		}
	}
}

void UCombatComponent::SoftLockOnUpdate()
{
	//if (LockOnMode == ELockOnType::ELT_SoftLockOn || LockOnMode==ELockOnType::ELT_None)
	//{
	//	EngageLockOn();

	//	if (IsValid(TargetActor))
	//	{
	//		SoftLockOn();
	//	}
	//	else
	//	{
	//		DisEngageLockOn();
	//	}
	//}
}

void UCombatComponent::SwitchLockedTarget(bool _bLeft)
{
	if (true == IsLockOnIngaged && IsValid(TargetActor))
	{
		if (LockOnTargets.Num() >= 2)
		{
			bool CanSwitch = false;

			AActor* StoredLockOnTarget = TargetActor;
			float SwitchDistance = 100000.f;

			LockOnTargets.Remove(TargetActor);

			FVector MyLocation = Character->GetActorLocation();
			FVector MyRightVector = Character->GetActorRightVector();

			for (AActor* Target : LockOnTargets)
			{
				FVector TargetLocation = Target->GetActorLocation();

				FVector Direction = MyLocation - TargetLocation;
				Direction.Normalize();

				double DotValue = UKismetMathLibrary::Dot_VectorVector(MyRightVector, Direction);

				//if less than zero, enemy is exist left of the player
				if (true == _bLeft)
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

			if (true == CanSwitch)
			{
				if (IsValid(TargetActor))
				{
					ATNHCharacter* Target = Cast<ATNHCharacter>(TargetActor);
					if (Target)
					{
						Target->GetCombatComponent()->SetVisibilityHPBar(true);
					}
				}

				if (IsValid(StoredLockOnTarget))
				{
					ATNHCharacter* Target = Cast<ATNHCharacter>(StoredLockOnTarget);
					if (Target)
					{
						Target->GetCombatComponent()->SetVisibilityHPBar(false);
					}
				}
				LockOnTargets.AddUnique(StoredLockOnTarget);

			}
		}
	}
}

void UCombatComponent::ToggleBattleMode()
{
	if (IsValid(MainWeapon))
	{
		if (false == WeaponEquiped)
		{
			PlayEquipAnim();
		}
		//else
		//{
		//	PlayUnEquipAnim();
		//}
	}
}

void UCombatComponent::Interaction()
{
	if (Character)
	{
		FVector CharacterLocation = Character->GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
		ObjectType.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

		FHitResult HitResult;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, CharacterLocation, CharacterLocation, 100.f, ObjectType, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
		{
			if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), UInteractableInterface::StaticClass()))
			{
				IInteractableInterface::Execute_Interact(HitResult.GetActor(), Character);
			}
		}
	}
}

void UCombatComponent::Attack()
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("%s"), *GetEnumDisplayName(CurrentState)));

		switch (Character->CurrentState)
		{
		case ECharacterState::Idle:
		{
			ComboStart();
		}
			break;
		case ECharacterState::Attack:
		{
			if (IsCheckingCombo)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("LightAttack")));
				IsContinueCombo = true;
				StoredNextAttack = EAttackType::EAT_LightAttack;
			}
		}
			break;
		case ECharacterState::Missed:
			break;
		case ECharacterState::Dodge:
			break;
		case ECharacterState::Guard:
			break;
		case ECharacterState::Dead:
			break;
		}
	}
}

void UCombatComponent::Charge(bool _bClicked)
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		if (_bClicked)
		{
			if (Character->CurrentState == ECharacterState::Idle)
			{
				PlayChargingAttackAnim(EPlayType::EPT_Start);
			}
		}
		else
		{
			if (IsCharging)
			{
				EndCharge();
				Attack();
			}
		}
	}
}

void UCombatComponent::Dodge()
{
	if (IsValid(Character))
	{
		if (CanAttack())
		{
			IsDodging = true;
			PlayDodgeAnim();
		}
	}
}

void UCombatComponent::Block(bool _bClicked)
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		if (CanAttack() && _bClicked)
		{
			IsBlocking = true;
			Character->CurrentState = ECharacterState::Guard;
			Character->GetCharacterMovement()->MaxWalkSpeed = 75.f;
			PlayBlockAnim();
		}
		else if(IsBlocking==true && _bClicked==false)
		{
			IsBlocking = false;
			Character->CurrentState = ECharacterState::Idle;
			Character->GetCharacterMovement()->MaxWalkSpeed = 375.f;
			PlayBlockAnim(true);
		}
	}
}

void UCombatComponent::BreakAttack()
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		if (IsCheckingCombo)
		{
			StoredNextAttack = EAttackType::EAT_BreakAttack;
		}

		if (Character->CurrentState == ECharacterState::Idle)
		{
			PlayBreakAttackAnim();
		}
	}
}

void UCombatComponent::ComboStart()
{
	CurrentComboDir = CurrentDirection;
	CurrentComboIndex = 0;

	switch (StoredNextAttack)
	{
	case EAttackType::EAT_None:
		PlayAttackComboAnim(CurrentComboIndex++);
		break;
	}

	Character->OnNormalAttackStarted.Broadcast();
}

void UCombatComponent::CheckNextCombo()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ComboCheckBegin")));

	IsContinueCombo = false;
	IsCheckingCombo = true;

	Character->CurrentState = ECharacterState::Attack;
	StoredNextAttack = EAttackType::EAT_None;
}

void UCombatComponent::ContinueCombo()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ComboContinue")));
	if (StoredNextAttack!=EAttackType::EAT_None)
	{
		switch (StoredNextAttack)
		{
		case EAttackType::EAT_LightAttack:
			PlayAttackComboAnim(CurrentComboIndex++);
			break;
		case EAttackType::EAT_HeavyAttack:
			break;
		case EAttackType::EAT_BreakAttack:
			PlayBreakAttackAnim();
			break;
		}
	}

	IsCheckingCombo = false;
}

void UCombatComponent::ResetState(UAnimMontage* _Montage, bool _bInterrupt)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ComboEnd")));

	ChargingTime = 0.f;

	IsAttacking = false;
	IsCheckingCombo = false;
	IsContinueCombo = false;
	IsChargeCombo = false;

	StoredNextAttack = EAttackType::EAT_None;
	Character->CurrentState = ECharacterState::Idle;
}

void UCombatComponent::EndDodge()
{
	IsDodging = false;
}

void UCombatComponent::StartCharge()
{
	
}

void UCombatComponent::UpdateCharge(float _delta)
{
	
}

void UCombatComponent::EndCharge()
{
	
}

FName UCombatComponent::GetWeaponSocket(AWeaponBase* _Weapon, bool _bEquip)
{
	FName SocketName;

	switch (_Weapon->GetWeaponType())
	{
	case EWeaponType::EWT_LongSword:
	{
		if (_bEquip) SocketName = FName("LongSwordHandSocket");
		else SocketName = FName("SwordHipSocket");
	}
		break;
	case EWeaponType::EWT_GreatSword:
	{
		if (_bEquip) SocketName = FName("GreatSwordHandSocket");
		else SocketName = FName("GreatSwordBackSocket");
	}
		break;
	}

	return SocketName;
}

UAnimMontage* UCombatComponent::GetWeaponEquipAnimation(AWeaponBase* _Weapon, bool _bEquip)
{
	UAnimMontage* EquipAnim = nullptr;

	switch (_Weapon->GetWeaponType())
	{
	case EWeaponType::EWT_LongSword:
	{
		if (_bEquip) EquipAnim = LongSwordEquipAnim;
		else EquipAnim = LongSwordUnEquipAnim;
	}
	break;
	case EWeaponType::EWT_GreatSword:
	{
		if (_bEquip) EquipAnim=GreatSwordEquipAnim;
		else EquipAnim=GreatSwordUnEquipAnim;
	}
	break;
	}

	return EquipAnim;
}

UAnimMontage* UCombatComponent::GetWeaponAttackAnimation(AWeaponBase* _Weapon, EDirection _Dir)
{
	UAnimMontage* AttackAnim = nullptr;

	switch (_Weapon->GetWeaponType())
	{
	case EWeaponType::EWT_LongSword:
	{
		switch (_Dir)
		{
		//case EDirection::ED_Up:
		//{
		//	if (LongSwordUpComboAnims.Num() > _Index && _Index >= 0)
		//	{
		//		AttackAnim = LongSwordUpComboAnims[_Index];
		//	}
		//}
		//	break;
		//case EDirection::ED_Left:
		//{
		//	if (LongSwordLeftComboAnims.Num() > _Index && _Index >= 0)
		//	{
		//		AttackAnim = LongSwordLeftComboAnims[_Index];
		//	}
		//}
		//	break;
		//case EDirection::ED_Right:
		//{
		//	if (LongSwordRightComboAnims.Num() > _Index && _Index >= 0)
		//	{
		//		AttackAnim = LongSwordRightComboAnims[_Index];
		//	}
		//}
			break;
		case EDirection::ED_None:
		{
			AttackAnim = LongSwordComboAnim;
		}
			break;
		}
	}
	break;
	case EWeaponType::EWT_GreatSword:
	{
	}
	break;
	}

	return AttackAnim;
}

TArray<class UAnimMontage*> UCombatComponent::GetWeaponAttackAnimationArray(AWeaponBase* _Weapon, EDirection _Dir)
{
	TArray<class UAnimMontage*> AttackAnims;

	switch (_Weapon->GetWeaponType())
	{
	case EWeaponType::EWT_LongSword:
	{
		switch (_Dir)
		{
		case EDirection::ED_Up:
			AttackAnims = LongSwordUpComboAnims;
			break;
		case EDirection::ED_Left:
			AttackAnims = LongSwordLeftComboAnims;
			break;
		case EDirection::ED_Right:
			AttackAnims = LongSwordRightComboAnims;
			break;
		}
	}
	break;
	case EWeaponType::EWT_GreatSword:
	{
	}
	break;
	}

	return AttackAnims;
}

void UCombatComponent::InitOnWeaponEquip()
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		TArray<FName> Sockets = Character->GetDidItHitComponent()->SetupVariables(MainWeapon->GetItemMesh(), nullptr);

		for (auto SocketName : Sockets)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%s"), *SocketName.ToString()));
		}
	}
}

bool UCombatComponent::CanAttack()
{
	bool Check = IsAttacking == false 
		&& IsDodging == false;

	return Check;
}

void UCombatComponent::OnSwordHit(FHitResult _LastItem)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("Trace Hit %s"), *_LastItem.GetActor()->GetName()));

	TSubclassOf <UDamageType> DamageType = UDT_LongSwordAttack::StaticClass();

	UDT_LongSwordAttack* LongSword = Cast<UDT_LongSwordAttack>(DamageType.Get()->GetDefaultObject());

	if (LongSword)
	{
		LongSword->SetDamageReactionType(AttackReaction);
		LongSword->SetKnockbackVector(KnockbackVector);
	}

	UGameplayStatics::ApplyPointDamage(_LastItem.GetActor(), 200.f, _LastItem.ImpactNormal, _LastItem, Character->GetInstigatorController(), Character, DamageType);
}

void UCombatComponent::SetHPBar(UWidgetComponent* _HPBar)
{
	HPBar = _HPBar;
}

//void UCombatComponent::SetDirectionIndicator(UDirectionIndicator_Widget* _DirectionIndicator)
//{
//	DirectionIndicator = _DirectionIndicator;
//}
//
//void UCombatComponent::DetectAttackDirection()
//{
//	if (Character && DirectionIndicator)
//	{
//		float MouseX, MouseY;
//		GetWorld()->GetFirstPlayerController()->GetInputMouseDelta(MouseX, MouseY);
//
//		if (MouseY > 0.0f)
//		{
//			CurrentDirection = EDirection::ED_Up;
//
//			DirectionIndicator->SetDirLeftColor(false);
//			DirectionIndicator->SetDirRightColor(false);
//		}
//		else
//		{
//			if (MouseX > 0.0f)
//			{
//				CurrentDirection = EDirection::ED_Right;
//
//				DirectionIndicator->SetDirLeftColor(false);
//				DirectionIndicator->SetDirRightColor(true);
//			}
//			else if (MouseX < 0.0f)
//			{
//				CurrentDirection = EDirection::ED_Left;
//
//				DirectionIndicator->SetDirLeftColor(true);
//				DirectionIndicator->SetDirRightColor(false);
//			}
//		}
//	}
//}

void UCombatComponent::SetVisibilityHPBar(bool _Visible)
{
	HPBar->SetVisibility(_Visible);
}

void UCombatComponent::PlayEquipAnim()
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		UAnimMontage* EquipAnim = GetWeaponEquipAnimation(MainWeapon, true);

		if (EquipAnim)
		{
			Character->GetCharacterAnimInstance()->Montage_Play(EquipAnim);
		}
	}
}

void UCombatComponent::PlayUnEquipAnim()
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		UAnimMontage* EquipAnim = GetWeaponEquipAnimation(MainWeapon, false);

		if (EquipAnim)
		{
			Character->GetCharacterAnimInstance()->Montage_Play(EquipAnim);
		}
	}
}

void UCombatComponent::PlayAttackComboAnim(int32 _Index)
{
	if (IsValid(Character) && IsValid(MainWeapon))
	{
		UAnimMontage* AttackAnim = GetWeaponAttackAnimation(MainWeapon);

		if (AttackAnim)
		{
			FOnMontageEnded OnEnded;
			FRotator CharacterYaw = Character->GetActorRotation();

			CharacterYaw.Yaw = GetWorld()->GetFirstPlayerController()->GetControlRotation().Yaw;
			//Character->SetActorRotation(CharacterYaw);

			OnEnded.BindUObject(this, &UCombatComponent::ResetState);

			if (Character->GetCharacterAnimInstance()->Montage_IsPlaying(AttackAnim) == false)
			{
				Character->GetCharacterAnimInstance()->Montage_Play(AttackAnim);
				Character->CurrentState = ECharacterState::Attack;
				Character->GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, AttackAnim);
			}
			else
			{
				FString Temp = "Combo";
				Temp.Append(FString::FromInt(_Index + 1));

				Character->CurrentState = ECharacterState::Attack;
				Character->GetCharacterAnimInstance()->Montage_JumpToSection(FName(Temp), AttackAnim);
			}
		}
	}
}

void UCombatComponent::PlayChargeComboAnim(int32 _Index)
{
}

void UCombatComponent::PlayDodgeAnim()
{
	if (IsValid(Character))
	{
		if (DodgeAnims.Num() > 0)
		{
			int32 MontageIndex = FMath::RandRange(0, DodgeAnims.Num() - 1);

			UAnimMontage* DodgeAnim = DodgeAnims[MontageIndex];

			Character->GetCharacterAnimInstance()->Montage_Stop(0.3f);

			if (DodgeAnim)
			{
				Character->GetCharacterAnimInstance()->Montage_Play(DodgeAnim);
			}
		}
	}
}

void UCombatComponent::PlayBlockAnim(bool _bOut)
{
	if (IsValid(Character))
	{
		if (BlockAnim)
		{
			if (_bOut == false)
			{
				Character->GetCharacterAnimInstance()->Montage_Play(BlockAnim);
			}
			else
			{
				if (Character->GetCharacterAnimInstance()->Montage_IsPlaying(BlockAnim))
				{
					Character->GetCharacterAnimInstance()->Montage_JumpToSection(FName("End"), BlockAnim);
				}
			}
		}
	}
}

void UCombatComponent::PlayBlockCounterAnim()
{
	if (IsValid(Character))
	{
		if (BlockCounterAnim)
		{
			Character->GetCharacterAnimInstance()->Montage_Play(BlockCounterAnim);
		}
	}
}

void UCombatComponent::PlayBreakAttackAnim()
{
	if (IsValid(Character) && BreakAttackAnim)
	{
		FOnMontageEnded OnEnded;
		OnEnded.BindUObject(this, &UCombatComponent::ResetState);

		Character->GetCharacterAnimInstance()->Montage_Play(BreakAttackAnim);

		Character->CurrentState = ECharacterState::Attack;

		Character->GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, BreakAttackAnim);
	}
}

void UCombatComponent::PlayChargingAttackAnim(EPlayType _PlayType)
{
	if (IsValid(Character))
	{
		if (ChargingAttackAnim)
		{
			if (_PlayType == EPlayType::EPT_Start)
			{
				FOnMontageEnded OnEnded;
				OnEnded.BindUObject(this, &UCombatComponent::ResetState);

				Character->GetCharacterAnimInstance()->Montage_Play(ChargingAttackAnim);
				Character->GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, ChargingAttackAnim);
			}
			else
			{
				if (Character->GetCharacterAnimInstance()->Montage_IsPlaying(ChargingAttackAnim))
				{
					if (_PlayType == EPlayType::EPT_Cancel)
					{
						Character->GetCharacterAnimInstance()->Montage_JumpToSection(FName("Cancel"), ChargingAttackAnim);
					}
					else if (_PlayType == EPlayType::EPT_Attack)
					{
						Character->GetCharacterAnimInstance()->Montage_JumpToSection(FName("Attack"), ChargingAttackAnim);
					}
				}
			}
		}
	}
}
