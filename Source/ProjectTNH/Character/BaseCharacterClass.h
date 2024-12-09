// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSCharacter.h"
#include "../DataTable/CharacterStatusData.h"
#include "UObject/Interface.h"
#include "ProjectTNH/Interfaces/LockOnInterface.h"
#include "ProjectTNH/Character/CharacterState.h"
#include "ProjectTNH/Character/PawnType.h"
#include "DidItHitActorComponent.h"
#include "ProjectTNH/DataTable/DT_AnimationTable.h"
#include "ProjectTNH/DataTable/DT_AttackRow.h"
#include "ProjectTNH/DataTable/DT_ExecutionData.h"
#include "ProjectTNH/CustomSequences.h"
#include "BaseCharacterClass.generated.h"

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front UMETA(DisplayName = "Front"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Rear UMETA(DisplayName = "Rear"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackBlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHittedDele, float, _Value);
//DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnParryHited, AActor*, _ParryCauser, float, _BalanceDamage);

UCLASS()
class PROJECTTNH_API ABaseCharacterClass : public AALSCharacter, public ILockOnInterface
{
	GENERATED_BODY()
private :
	FTimerHandle Straggerhandle;

protected:
	FCharacterStatusTableRow* CharacterInfo;
	float RecoveredTime = 10.0f;

public:
	// Sets default values for this pawn's properties
	ABaseCharacterClass(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	AActor* MissionObject;

	UFUNCTION(BlueprintCallable, Category = "Execution")
	float GetCurrentRecoveredTime();

	UFUNCTION(BlueprintCallable, Category = "Execution")
	float GetRecoveredTime();

	UFUNCTION(BlueprintCallable, Category = "Execution")
	float GetRecvoreValue();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float balance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float Maxbalance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float threat = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float Walkspeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float LockOnspeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT_CharacterStatus")
	float runSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threat")
	float DefaultThreat;

	UPROPERTY(VisibleAnywhere)
	bool bIsAttackCoolTime = false;

	UPROPERTY()
	bool bIsGuardable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	int32 TeamNum = 0;

	UPROPERTY()
	bool bIsSuperArmor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState CurrentState = ECharacterState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombatDirection CurrentDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* SB_WidgetComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* Dr_widgetComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "PawnType")
	EPawnType pawnType = EPawnType::NormalSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnType")
	float GuardDegree = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* LockOnPoint;

	bool bShouldMoveToTarget = false;

	float MoveSpeed;

	bool HitIgnore = false;

	bool bShouldTurnToTarget = false;

	FVector BeforeDodgeLocation;

	bool IsPlayerTarget;

	virtual void ClearDataAll();

	struct FAnimationTable* CharacterAnimationData;

	class UDataTable* AttackInfoData;

	class UDataTable* ExecutionInfoData;

	class UDataTable* ETCTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDidItHitActorComponent* HitComponents = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> SpawnWeaponClass;

	UAnimMontage* CurrentKnockBackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AWeaponBase* MainWeapon;

	UPROPERTY(EditAnywhere)
	class UAIPerceptionStimuliSourceComponent* Stimulisoure = nullptr;

	UPROPERTY(EditAnywhere)
	class UMotionWarpingComponent* MotionWarping;

	EReactionType AttackReaction;

	float DamageAmount;

	float BalanceAmount;

	float BlockBalanceAmount;

	float ParryBalanceAmount;

	float HitReactPlaySecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CapsuleLockOnRadius = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CapsuleNormalRadius = 35.f;

	ECombatDirection AttackDirection;

	bool IsKeepGuard = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsRecoverdBalance = false;

	UDataTable* AttackTable = nullptr;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class AActor* TargetActor;

	UPROPERTY(EditAnywhere)
	float MinTrackDistance = 150.f;

	UPROPERTY(EditAnywhere)
	float MaxTrackDistance = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackTrackDist = 150.f;

	float AttackStartTime = 0;

	FVector OriginTrackingPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABaseCharacterClass* LastBeAttackedCharacter;

	ABaseCharacterClass* LastestApplyDamagePawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere)
	class UTargetSelectComponent* _TargetSelectComponents = nullptr;

	UFUNCTION()
	virtual void OnSwordHit(FHitResult _LastItem);

	virtual UAnimMontage* LowDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f);

	virtual UAnimMontage* GuardBreakReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f);

	virtual UAnimMontage* HighDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f);

	virtual UAnimMontage* KnockBackReaction(AActor* _DamageCauser, FVector _DirVector);

	virtual UAnimMontage* GuardReaction(AActor* _DamageCauser, ECombatDirection _Direction);

	virtual UAnimMontage* DeathReaction(AActor* _DamageCauser, int _Direction);

public:
	virtual UAnimMontage* GroggyReaction(AActor* _DamageCauser, int _Direction);

protected:
	virtual void DoParring(ECombatDirection _Direction, EReactionType _ReactionType);

	UFUNCTION()
	void KnockBackCheckRagdoll();

	bool CheckBalanceFromPawnType();

	void InitNextAttackData(int32 _Index, ECombatDirection _Direction, bool _bTargetExist = true);

	void InitMotionWarpingData(AActor* _AttackTarget);

public:
	void UpdateAttackLocation();

	void StopAttackTracking();

	void SetLockedCollisionRadius(ABaseCharacterClass* _Target, bool _Locked);

protected:
	bool bStartTracking = false;

	float MoveDistance;

	class ABaseCharacterClass* TrackAttackTarget;

public:
	struct FDT_ETC* ETCData = nullptr;
	struct FAttackRow* AttackData = nullptr;

	UFUNCTION()
	virtual void InitData();

	UPROPERTY(EditAnywhere)
	float SearchDistance = 400.f;

	UPROPERTY(EditAnywhere)
	TArray<class UNiagaraSystem*> BloodEffectAry;

	UFUNCTION(BlueprintCallable)
	void SetSuperArmor(bool Active) { Active = bIsSuperArmor; }

	UFUNCTION()
	void SetColiisionSetting();

	void SpawnMainWeapon();

	struct FAttackRow* GetAttackInfoData(int _index, ECombatDirection _Direction, bool _bTargetExist = true);

	UFUNCTION()
	void SettingPawnDataTable();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite);
	class UUserWidget* StateBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentComboIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxComboIndex = 0;

	UFUNCTION()
	void SettingStatusPawn();

	UFUNCTION()
	virtual void ApplyDamageBalance(float Damage);

	UFUNCTION()
	virtual void RecoverdBalace();

	UFUNCTION()
	FString GetPawnType();

	UFUNCTION(BlueprintCallable)
	inline ECombatDirection GetCurrentGuardDriection() { return CurrentDirection; }

	UFUNCTION()
	void SetCurrentDirection(ECombatDirection _Dir) { CurrentDirection = _Dir; }

	UFUNCTION()
	bool CheckGuardableDirection(ECombatDirection _Direction, EReactionType _ReactionType);

	UFUNCTION()
	bool CheckGuardSuccess(AActor* Enemy);

	UFUNCTION()
	virtual void OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage);

	UFUNCTION()
	bool IsCanAction();

	UFUNCTION()
	bool bIsDeadPawn();
	
	UFUNCTION()
	FORCEINLINE UAnimInstance* GetCharacterAnimInstance() { return GetMesh()->GetAnimInstance(); }

	FORCEINLINE class UDidItHitActorComponent* GetDidItHitComponent() const { return HitComponents; }

	FORCEINLINE struct FAnimationTable* GetAnimData() { return CharacterAnimationData; }

	FORCEINLINE void SetTargetActor(AActor* _target) { TargetActor = _target; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetTargetActor() { return TargetActor; }
	FORCEINLINE UTargetSelectComponent* GetTargetSelectComponents() { return _TargetSelectComponents; }

	UFUNCTION(BlueprintCallable)
	void SetCharacterState(ECharacterState _NewState) { CurrentState = _NewState; }


	UFUNCTION()
	void SetFixedRagdoll();

	UFUNCTION()
	void SetCharacterWalkSpeed(float _Value);

	virtual void TrackingTarget(float _Delta, float _MoveSpeed);

	virtual void TurnToTarget(float _Delta);
	class ABaseCharacterClass* Execution_Causer;

	bool IsMinion();

	virtual void OnPushed();

protected:
	void PlayGuardAnim(bool _End = false);
	void ResumeGuardAnim(UAnimMontage* _Montage, bool _bInterrupt);

	class ABaseCharacterClass* Execution_Target;

	class ABaseCharacterClass* CurrentExecution_Target;

	virtual void FindExecutionTarget();

	bool CanExecution(AActor* _Target);

public :
	UFUNCTION(BlueprintImplementableEvent)
	void OnHitted(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecuted(AActor* DamageCauser);

	FDoOnce DoOnce;

	void Execution_Play(int32 _index = -1);

	virtual void Execution_Attacker(int32 _index);

	virtual void Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance);

	bool ExecutionAble = true;

	UFUNCTION()
	virtual void OnEnded_Attacker(UAnimMontage* _VictimMontage, bool _Interrupt);

	UFUNCTION()
	virtual void OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt);

	UFUNCTION()
	virtual void OnEnded_ParryHitReaction(UAnimMontage* _HitReaction, bool _Interrupt);

	UFUNCTION()
	virtual void OnEnded_ParryStunReaction(UAnimMontage* _HitReaction, bool _Interrupt);

	UFUNCTION(BlueprintCallable)
	int GetTeamNumber() { return TeamNum; }

	UFUNCTION()
	void ChangeControllerRotation(bool flag);

	FString GetAttackName();

public:
	FOnDeaded OnDeaded;
	FOnAttackBlocked OnAttackBlocked;
	//FOnParryHited OnParryHited;

public:
	class AWeaponBase* GetMainWeapon() { return MainWeapon; }

	UFUNCTION()
	void DestroyMainWeapon();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Blueprint_DeathCall();

};
