// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSecondPhase.h"
#include "ProjectTNH/DamageType/DT_LongSwordAttack.h"
#include "ProjectTNH/TNHGameInstance.h"
#include "ProjectTNH/UI/CustomWidget_PlayerStateBarBase.h"
#include "ProjectTNH/PlayerController/TNHPlayerController.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterController.h"
#include "ProjectTNH/AI/Ai_CharacterBase/Ai_CharacterBase.h"
#include "ProjectTNH/UI/HUD/TNHMainHUD.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "ProjectTNH/AI/AI_DataTable/DT_AiETCDataRow.h"
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

void ABossSecondPhase::BeginPlay()
{
	Super::BeginPlay();

	ExecutionDeathCount = FMath::CeilToInt(1.f / ExecutionDamagePercent);
}

float ABossSecondPhase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void ABossSecondPhase::Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance)
{
	if (ExecutionCount == ExecutionDeathCount - 1)
	{
		OnBossDead();

		CurrentState = ECharacterState::Dead;

		AAi_CharacterController* AI_Controller = Cast<AAi_CharacterController>(GetController());

		if (AI_Controller)
		{
			AI_Controller->StopMovement();
		}
	}
	else
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

		CurrentState = ECharacterState::ExecutionRecovery;

		OnExecutionStart();

		AAi_CharacterController* AI_Controller = Cast<AAi_CharacterController>(GetController());

		if (AI_Controller)
		{
			AI_Controller->StopMovement();
		}

		GetCharacterAnimInstance()->Montage_SetEndDelegate(OnEnded, _VictimMontage);
	}
}

void ABossSecondPhase::OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt)
{
	if (ExecutionCount < ExecutionDeathCount - 1)
	{
		//GetCharacterAnimInstance()->Montage_Play(CharacterAnimationData->Stun_Reaction[0]);

		//GetMesh()->SetCollisionProfileName(FName("CharacterMesh"));
		GetCapsuleComponent()->SetCollisionProfileName(FName("IgnoreOnlyPawn"));
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

		balance = Maxbalance;
		CurrentState = ECharacterState::Idle;

		//if (!GetWorld()->GetTimerManager().IsTimerActive(ExecutionStunTimer))
		//{
		//	GetWorld()->GetTimerManager().SetTimer(ExecutionStunTimer, FTimerDelegate::CreateLambda([&]()
		//		{

		//			if (CurrentState == ECharacterState::ExecutionRecovery)
		//			{
		//				CurrentState = ECharacterState::Idle;

		//				bIsRecoverdBalance = false;
		//			}
		//		}), ExecutionStunRecoveryTime, false);
		//}

		ExecutionCount++;
		Health -= MaxHealth * ExecutionDamagePercent;

		ATNHMainHUD* mainHUD = Cast<ATNHMainHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

		if (mainHUD)
		{
			mainHUD->SettingStateBar(Health, MaxHealth, false, StateBar);
			mainHUD->SettingStateBar(balance, Maxbalance, true, StateBar);
		}

		OnExecutionEnd();

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("BossExecution : %d / %d"), ExecutionDeathCount, ExecutionCount));
	}
	//else
	//{
	//	//BossPhase End

	//	CurrentState = ECharacterState::Dead;

	//	OnBossDead();

	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("BossExecution : %d / %d, Boss Dead"), ExecutionDeathCount, ExecutionCount));
	//}
}
