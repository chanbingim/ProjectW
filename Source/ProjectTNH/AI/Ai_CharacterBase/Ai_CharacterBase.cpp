// Fill out your copyright notice in the Description page of Project Settings.

#include "Ai_CharacterBase.h"
#include "GameFramework/Character.h"
#include "Ai_CharacterController.h"
#include "AiCharacter_AnimBase.h"
#include "../UAiCommander.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../UI/HUD/TNHMainHUD.h"
#include "ProjectTNH/DataTable/DT_AnimationTable.h"
#include "ProjectTNH/AttackListComponents.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../AiMoveComponents.h"
#include "Engine/DamageEvents.h"
#include "Components/DecalComponent.h"
#include "../../TNHGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../../UI/WB_StateBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TargetSelectComponent.h"
#include "NavigationSystem.h"
#include "Components/WidgetComponent.h"
#include "ProjectTNH/ActorUtiliyCompoents.h"
#include "ProjectTNH/TNHGameModeBase.h"
#include "ProjectTNH/UI/DirectionIndicator_Widget.h"
#include "ProjectTNH/ChangeWeaponComponents.h"
#include "ProjectTNH/UI/CustomWidget_PlayerStateBarBase.h"


AAi_CharacterBase::AAi_CharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	_AttackListComponents = CreateDefaultSubobject<UAttackListComponents>(TEXT("AttackListComponents"));
	_MoveComponents = CreateDefaultSubobject<UAiMoveComponents>(TEXT("MoveComponents"));

	//GetMesh()->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

// Called when the game starts or when spawned
void AAi_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

}
 
void AAi_CharacterBase::InitData()
{
	Super::InitData();

	if(GetController() != nullptr)
		Ai_Controller = Cast<AAiBaseController>(GetController());
	
	SettingLeftWeapon();
	if (Ai_Controller != nullptr)
	{
		Ai_Controller->SetAiStatusData(GetPawnType());
	}

	if(_MoveComponents != nullptr)
		_MoveComponents->SetOwnerActor(this);

	ChangeControllerRotation(false);

	SetCurrentDirection(ECombatDirection::Right);
}

void AAi_CharacterBase::ClearDataAll()
{
	if (CurrentState == ECharacterState::Dead)
		return;
	
	if (CurrentState == ECharacterState::Move)
		FOnMoveDel.Broadcast();

	if (CurrentState == ECharacterState::Attack || bIsAttackCoolTime)
	{
		IAttackEnd();
	}

	else if (CurrentState == ECharacterState::Stun)
	{
		ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

		if(StateBar)
			StateBar->SetVisibility(ESlateVisibility::Visible);
		
		bIsAttackCoolTime = true;

		if(!GetWorld()->GetTimerManager().IsTimerActive(PostGroggyAttackDelay))
		{
			GetWorld()->GetTimerManager().SetTimer(PostGroggyAttackDelay, FTimerDelegate::CreateLambda([&]()
				{
					if (!IsValid(this))  // 객체 유효성 검사
					{
						return;  // 객체가 유효하지 않으면 실행 중단
					}

					bIsAttackCoolTime = false;
					GetWorld()->GetTimerManager().ClearTimer(PostGroggyAttackDelay);
				}), PostGroggyDelay, false);
		}
		
		CurrentState = ECharacterState::Idle;
	}
}

UAnimMontage* AAi_CharacterBase::DeathReaction(AActor* _DamageCauser, int _Direction)
{
	Super::DeathReaction(_DamageCauser, _Direction);

	if (CurrentState == ECharacterState::Dead)
		return nullptr;

	auto Causer = Cast<ABaseCharacterClass>(_DamageCauser);
	auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();

	if (Causer->TargetActor == this)
	{
		Causer->TargetActor = nullptr;
	}

	Causer->GetTargetSelectComponents()->RemoveHostileData(this);

	DeathRemoveComponents();
	CurrentState = ECharacterState::Dead;
	
	OnDeaded.Broadcast();

	if (Commander != nullptr)
		Commander->AiChangedStateToDead(this);

	if (Ai_Controller != nullptr)
	{
		Ai_Controller->DeadAiPawn(_DamageCauser);
	}
		
	
	return nullptr;
}

UAnimMontage* AAi_CharacterBase::GroggyReaction(AActor* _DamageCauser, int _Direction)
{
	UAnimMontage* SelectedMontage = Super::GroggyReaction(_DamageCauser, _Direction);
	
	if (balance > 0)
		return SelectedMontage;

	if (CurrentState == ECharacterState::Attack)
		IAttackEnd();

	CurrentState = ECharacterState::Stun;
	Ai_Controller->SetFocus(nullptr);

	if (StateBar != nullptr)
	{
		ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
		StateBar->SetVisibility(ESlateVisibility::Collapsed);

		auto DirectionWidget = GameMainHUD->FindCustomWidgetFromWidgetPool(this);
		if (DirectionWidget != nullptr)
			GameMainHUD->RemoveCustomWidgetFromWidgetPool(DirectionWidget);
	}

	return SelectedMontage;
}

void AAi_CharacterBase::OnEnded_ParryStunReaction(UAnimMontage* _HitReaction, bool _Interrupt)
{
	Super::OnEnded_ParryStunReaction(_HitReaction, _Interrupt);

	ClearDataAll();
	CurrentState = ECharacterState::Idle;
}

void AAi_CharacterBase::Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance)
{
	Super::Execution_Victim(_ExecutionAttacker, _VictimMontage, _BetweenDistance);

	AAi_CharacterController* AI_Controller = Cast<AAi_CharacterController>(GetController());

	if (AI_Controller)
	{
		AI_Controller->StopMovement();
	}
}

void AAi_CharacterBase::OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt)
{
	Super::OnEnded_Victim(_VictimMontage, _Interrupt);

	if (StateBar != nullptr)
	{
		ATNHMainHUD* GameMainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
		StateBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();

	if (Execution_Causer && Commander)
	{
		if (Execution_Causer->TargetActor == this)
		{
			Execution_Causer->TargetActor = nullptr;
		}

		Execution_Causer->GetTargetSelectComponents()->RemoveHostileData(this);
		CurrentState = ECharacterState::Dead;
		ResetTimer();
		Ai_Controller->UnPossess();
		OnDeaded.Broadcast();

		if (Commander != nullptr)
			Commander->AiChangedStateToDead(this);

		if (Ai_Controller != nullptr && Execution_Causer)
			Ai_Controller->DeadAiPawn(Execution_Causer);
	}
}

void AAi_CharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

//void AAi_CharacterBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	auto _HitActor = Cast<ABaseCharacterClass>(OtherActor);
//	if (nullptr == _HitActor)
//		return;
//
//	if (pawnType > EPawnType::NormalArcher || CurrentState == ECharacterState::Dead ||
//		!_HitActor->GetMesh()->ComponentHasTag("HitActor"))
//			return;
//	
//		
//
//	if (_HitActor->TeamNum == TeamNum && _HitActor->pawnType >= EPawnType::EliteSword)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%s"), *OtherActor->GetName()));
//
//		FVector ImpactDirection = (Hit.ImpactPoint - GetActorLocation()).GetSafeNormal(); // 충돌 방향 벡터
//		float Degree = UActorUtiliyCompoents::GetVectorAngle(GetActorForwardVector(), ImpactDirection);
//
//		int Index;
//
//		if (Enhanced_Push)
//			Index = UActorUtiliyCompoents::GetActorHitDirection8Way(Degree);
//		else
//			Index = UActorUtiliyCompoents::GetActorHitDirection4Way(Degree);
//
//		//animation index 재생
//		if (Index != -1)
//		{
//			if (CharacterAnimationData->Push_Reaction.Num() < Index)
//				return;
//
//			auto SelectedReaction = CharacterAnimationData->Push_Reaction[Index];
//			GetCharacterAnimInstance()->Montage_Play(SelectedReaction, 1.0f, EMontagePlayReturnType::MontageLength, AttackStartTime);
//		}
//	}
//}

// Called every frame
void AAi_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAi_CharacterBase::OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage)
{
	Super::OnParryHited_Func(_ParryCauser, _BalanceDamage);

	GetAttackListComponents()->ResetComboList();

	AAi_CharacterController* AI_Controller = Cast<AAi_CharacterController>(GetController());

	if (AI_Controller)
	{
		AI_Controller->StopMovement();
	}
}

void AAi_CharacterBase::SettingLeftWeapon()
{
	if (SubWeaponClass != nullptr)
	{
		FActorSpawnParameters Params;

		Params.Instigator = this;
		Params.Owner = this;

		AWeaponBase* TempWeapon = GetWorld()->SpawnActor<AWeaponBase>(SubWeaponClass, GetActorTransform(), Params);
		TempWeapon->OnInit();

		subWeapon = TempWeapon;
		subWeapon->OnEquipped();
	}
}

void AAi_CharacterBase::ResetData(float _value)
{
	if(CurrentState == ECharacterState::Stagger)
		IAttackEnd();
}

void AAi_CharacterBase::AttackSelect(int Index, AActor* _Target)
{
	bool bIsElite = pawnType >= EPawnType::EliteSword;

	if (CurrentState == ECharacterState::Dead || TargetActor == nullptr)
	{
		IAttackEnd();
		return;
	}

	AAi_CharacterController* OwnningController = Cast<AAi_CharacterController>(GetController());
	if (_AttackListComponents != nullptr && bIsElite)
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackCoolTime);
		if (OwnningController != nullptr && _Target == nullptr)
			_Target = Cast<AActor>(OwnningController->GetBlackboardComponent()->GetValueAsObject(AAi_CharacterController::Key_TargetEnemy));

		if (Index >= 0)
			AttackName = _AttackListComponents->GetCombolistIndex(CurrentComboIndex);
		else if (Index == -1)
			AttackName = _AttackListComponents->GetGuardBreakAttack();
		else
		{
			_AttackListComponents->MakeAttackComboList(1);
			AttackName = _AttackListComponents->GetCombolistIndex(CurrentComboIndex);
		}

		CurrentComboIndex += 1;
	}
	else
	{
		if(!bIsElite)
			AttackName = GetAttackName();
	}

	AttackData = AttackInfoData->FindRow<FAttackRow>(FName(AttackName), "");
	if (AttackData != nullptr)
	{
		AttackReaction = AttackData->ReactionType;
		AttackDirection = AttackData->AttackDirection;
		DamageAmount = AttackData->Damage;
		BalanceAmount = AttackData->BalanceDamage;
		BlockBalanceAmount = AttackData->BlockBalance;
		ParryBalanceAmount = AttackData->ParryBalance;
		AttackStartTime = AttackData->PostComboStartTime;
		HitReactPlaySecond = AttackData->HitReactPlaySecond;

		if (bIsElite)
			InitMotionWarpingData(TargetActor);
		
		AIAttackToPlayer(*AttackData);
	}

}

void AAi_CharacterBase::AIAttackToPlayer(FAttackRow Data)
{
	if (CurrentState == ECharacterState::Dead)
		return;

	CurrentDirection = AttackDirection;
	if (bIsAttackCoolTime || CurrentState == ECharacterState::Attack)
	{
		GetCharacterAnimInstance()->StopAllMontages(0);
		GetCharacterAnimInstance()->Montage_Play(Data.AttackAnimation, 1.0f,EMontagePlayReturnType::MontageLength, AttackStartTime);

		if (Ai_Controller != nullptr)
		{
			auto EliteController = Cast<AAi_CharacterController>(Ai_Controller);

			if(EliteController != nullptr)
				EliteController->GetBlackboardComponent()->SetValueAsBool(EliteController->Key_AttackCoolTime, bIsAttackCoolTime);
		}
	}
}

void AAi_CharacterBase::DeathRemoveComponents()
{
	ResetTimer();
	if (DecalComp != nullptr)
		DecalComp->DestroyComponent();

	if (_MoveComponents != nullptr)
		_MoveComponents->DestroyComponent();

	if(_AttackListComponents != nullptr)
		_MoveComponents->DestroyComponent();

	if (_TargetSelectComponents != nullptr)
		_TargetSelectComponents->DestroyComponent();
}

void AAi_CharacterBase::AddObjectPoolAndCommander()
{
	if (this == nullptr)
		return;

	auto GameMode = Cast<ATNHGameModeBase>(UGameplayStatics::GetGameMode(this));
	auto Commander = GetWorld()->GetSubsystem<UUAiCommander>();
	switch (TeamNum)
	{
	case 1:
		if (!Commander->WorldAllTeamPawn.Contains(this))
			Commander->WorldAllTeamPawn.Add(this);
		break;
	case 2:
		if (!Commander->WorldAllEnemyPawn.Contains(this))
			Commander->WorldAllEnemyPawn.Add(this);
		break;
	}
	GameMode->AddActorsToPoolOnBeginPlay(this);
}

void AAi_CharacterBase::ResetTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackCoolTime);
	GetWorld()->GetTimerManager().ClearTimer(PostGroggyAttackDelay);
}

void AAi_CharacterBase::IAttackEnd()
{
	if (CurrentState != ECharacterState::Dead && Ai_Controller != nullptr)
	{
		CurrentComboIndex = 0;
		if (!GetCharacterMovement()->bUseControllerDesiredRotation)
		{
			FTimerHandle DleayTime;
			TWeakObjectPtr<AAi_CharacterBase> WeakPtr = this;

			GetWorld()->GetTimerManager().SetTimer(DleayTime, FTimerDelegate::CreateLambda([WeakPtr](){
				if (WeakPtr.IsValid())  // 객체 유효성 검사
				{
					WeakPtr.Get()->ChangeControllerRotation(false);
				}
				
				}), 0.3f, false);
		}

		if (_AttackListComponents != nullptr)
			_AttackListComponents->ResetComboList();

		if (CurrentState == ECharacterState::Attack)
			CurrentState = ECharacterState::Idle;

		if (!GetWorld()->GetTimerManager().IsTimerActive(AttackCoolTime))
		{
			TWeakObjectPtr<AAi_CharacterBase> WeakPtr = this;

			if (Ai_Controller)
				CoolTime = Ai_Controller->PropertiesData->AttackCoolTime;
			else
				CoolTime = 1.0f;

			if (IsPlayerTarget)
			{
				auto EliteController = Cast<AAi_CharacterController>(Ai_Controller);
				SetLockedCollisionRadius(this, true);

				CurrentDirection = FMath::RandBool() ? ECombatDirection::Left : ECombatDirection::Right;

				if (EliteController != nullptr)
					EliteController->ChangeParringState(true);
			}

			GetWorld()->GetTimerManager().SetTimer(AttackCoolTime, FTimerDelegate::CreateLambda([WeakPtr]()
				{
					if (WeakPtr.IsValid())
					{
						auto TargetPtr = WeakPtr.Get();

						if (TargetPtr->CurrentState != ECharacterState::Dead && TargetPtr->Ai_Controller != nullptr)
						{
							auto EliteController = Cast<AAi_CharacterController>(TargetPtr->Ai_Controller);

							if (EliteController != nullptr && EliteController->GetBlackboardComponent() != nullptr)
								EliteController->GetBlackboardComponent()->SetValueAsBool(AAi_CharacterController::Key_AttackCoolTime, false);
						}

						TargetPtr->bIsAttackCoolTime = false;
						TargetPtr->GetWorld()->GetTimerManager().ClearTimer(TargetPtr->AttackCoolTime);
					}// 객체 유효성 검사
				
				}), CoolTime, false);
		}

		FOnAttackEnd.Broadcast();
	}
}

void AAi_CharacterBase::IHitEnd()
{
}
