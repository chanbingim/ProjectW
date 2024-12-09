// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterClass.h"
#include "StateMachine.h"
#include "../DataTable/DT_AttackProperties.h"
//#include "ProjectTNH/CustomSequences.h"
#include "TNHCharacter.generated.h"

/**
 *
 */
DECLARE_MULTICAST_DELEGATE(FOnStaminaRegenCancled);
DECLARE_MULTICAST_DELEGATE(FOnHPChanged);
DECLARE_MULTICAST_DELEGATE(FOnStaminaChanged);

DECLARE_MULTICAST_DELEGATE(FOnNormalAttackStarted);
DECLARE_MULTICAST_DELEGATE(FOnCrushAttackStarted);
DECLARE_MULTICAST_DELEGATE(FOnChargeStarted);
DECLARE_MULTICAST_DELEGATE(FOnGuardSuccessed);

DECLARE_MULTICAST_DELEGATE(FOnLowHP);
DECLARE_MULTICAST_DELEGATE(FOnEnoughHP);

DECLARE_MULTICAST_DELEGATE(FOnHited);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetFinded, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetLosted, AActor*);

class UWidgetComponent;

UENUM(BlueprintType)
enum class ELockOnType : uint8
{
	SoftLockOn UMETA(DisplayName = "SoftLockOn"),
	HardLockOn UMETA(DisplayName = "HardLockOn"),
};

UENUM(BlueprintType)
enum class EAttack
{
	NormalAttack UMETA(DisplayName = "NormalAttack"),
	ChargeAttack UMETA(DisplayName = "ChargeAttack"),
	CrushAttack UMETA(DisplayName = "CrushAttack"),
	None UMETA(DisplayNAme = "None")
};

UCLASS()
class PROJECTTNH_API ATNHCharacter : public ABaseCharacterClass
{
	GENERATED_BODY()

public:
	ATNHCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void DoParring(ECombatDirection _Direction, EReactionType _ReactionType) override;

	class ATNHPlayerController* Ownnigcontroller = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* BaseBody;

	float CurrentTimer;


protected:
	virtual void OnSwordHit(FHitResult _LastItem) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* sphereRange;

	UDataTable* AttackTable = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPostProcessComponent* DamagePostProcess;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void FindExecutionTarget() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ExecutionFindMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ExecutionDistanceApplyPoint = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ExecutionAngleApplyPoint = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ExecutionEliteApplyPoint = 10.f;

	void GetBoxCorners(FVector _Origin, FVector _BoxExtent, TArray<FVector>& Corners);

public:
	void DisableRootMotion(float _DisableRootMotionTime);
	void EnableRootMotion();

	void ResetRegenTimer();

	void StartRegen();

	UFUNCTION(BlueprintCallable)
	FRotator GetDesiredRotation();

	FORCEINLINE class UCombatComponent* GetCombatComponent() const { return Combat; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetHeadRotation() const { return HeadRotation; }

	FORCEINLINE void SetHeadRotation(bool _bRotate) { HeadRotation = _bRotate; }

	void SetHealth(float _Health);

	FORCEINLINE float GetHealth() const { return Health; }

	void SetMaxHealth(float _MaxHealth);

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	FORCEINLINE bool GetIsLockOnIngaged() const { return IsLockOnIngaged; }

	//FORCEINLINE float GetStamina() const { return Stamina; }

	//FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	float GetHealthRatio();

	FORCEINLINE float GetAxisTurn() const { return AxisTurn; }

	FORCEINLINE float GetAxisUp() const { return AxisUp; }

private:
	//LockOnSystem
	ELockOnType CurrentLockOnType = ELockOnType::SoftLockOn;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> LockOnTargets;

	FTimerHandle LockOnTargetUpdateTimer;

	FTimerHandle BreakLockTimer;

	FTimerHandle HPRegenTimer;

	float FindTargetRate = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsLockOnIngaged;

	bool ResumeLockOn;

	UFUNCTION(BlueprintCallable)
	void StartLockOn();

	void UpdateLockOnTargets();

	void UpdateLockOn();

	void EngageLockOn();

	void DisEngageLockOn();

	void BreakLockOn();

	void SetUpdateLockOnTargets(bool _Active);

	UFUNCTION(BlueprintCallable)
	void SwitchLockedTarget(bool _FindLeftTarget = true);

	bool IsTargetAble(ABaseCharacterClass* _NewTarget);

private:
	//Combat System
	bool IsCheckingKey = false;

	bool IsContinueAttack = false;

	bool bHealthRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinHealth = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordEquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordUnEquipAnim;

	UAnimMontage* CurrentParryCounterAnim;

	EAttack StoredNextAttack = EAttack::None;

	ECombatDirection StoredNextAttackDirection;

	FTimerHandle TrackingTimer;

	bool bTracking = false;

	bool bParryingSuccess = false;

	bool OnEndedGuardFuncSetted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool TiredState = false;

	float CombatAnimPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SideDodgeAmount = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RollDodgeAmount = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float NonCombatRollDodgeAmount = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AnimPlayRateChange = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DirectionSensitivity = 50.f;

	FVector2D MouseBeforePos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCancleAnimation = false;

	FDoOnce AlarmOnce_LowHP;

	FDoOnce AlarmOnce_EnoughHP;

	FGate HP_AlarmOnce;

	FDoOnce ParryCancleOnce;

	void StopAllAttackMontage(float _BlendTime);

	UPROPERTY(EditAnywhere)
	UAnimMontage* ParryCounter;

	void PlayNextAttackAnim();

	void PlayParryAnim(ECombatDirection _Direction);

	void PlayParryCounterAnim(ECombatDirection _Direction, EReactionType _ReactionType);

	void PlayDodgeAnim(int _Direction);

	void StartCombo();

	UFUNCTION(BlueprintCallable)
	void OnWaitingNextMove();

	void CancleAnimation(bool _bReverse);

	UFUNCTION()
	void PlayGuardLoopAnim(UAnimMontage* _Montage, bool _bInterrupt);

	UFUNCTION(BlueprintCallable)
	void ResetCombat(UAnimMontage* _Montage = nullptr, bool _bInterrupt = false);

	UFUNCTION()
	void ForceResetCombat();

	UFUNCTION()
	void OnHited_Func();

	UFUNCTION()
	void OnLowHP_Func();

	UFUNCTION()
	void OnEnoughHP_Func();

	UFUNCTION()
	void OnHPChanged_Func();

	UFUNCTION()
	void OnTargetDead_Func();

	UFUNCTION()
	void OnTargetFinded_Func(AActor* _Target);

	UFUNCTION()
	void OnTargetLosted_Func(AActor* _Target);

	UFUNCTION()
	void OnAttackBlocked_Func();

	void UpdateHealthRegen(float _Delta);

public:
	bool GetTargetExist() { return (TargetActor != nullptr); }

	UFUNCTION(BlueprintCallable)
	void CheckNextAttack();

	UFUNCTION(BlueprintCallable)
	void ContinueAttack();

	void UpdateDirectionIndicator();

	virtual void RecoverdBalace() override;

	virtual void ApplyDamageBalance(float Damage) override;

	bool IsNonCombatMode = true;

	bool bUseHandIk = true;

	FORCEINLINE bool GetUseHandIk() const { return bUseHandIk; }

	float NonCombatResumeTime = 3.f;

	FTimerHandle ResumeTimer;

	void ResumeNonCombatMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* NonCombatFirstAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* NonCombatFirstExecution;
	
private:
	//Key Actions
	UFUNCTION(BlueprintCallable)
	void Attack(bool _Down);

	UFUNCTION(BlueprintCallable)
	void Parry(bool _Down);

	UFUNCTION(BlueprintCallable)
	void Dodge();

	UFUNCTION(BlueprintCallable)
	void Crash(bool _Down);

	UFUNCTION(BlueprintCallable)
	void Ability(bool _Down);

	bool CheckExecution();

	UFUNCTION(BlueprintCallable)
	void Cancle();

private:
	FTimerHandle ResetRootMotionTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AxisTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AxisUp;

	bool LButtonDown = false;

	bool RButtonDown = false;

	bool WheelDown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ForwardScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RightScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AnimSpeedScale = 1.f;

	float DodgeCost;

public:
	bool GetLButtonDown() { return LButtonDown; }

	bool GetRButtonDown() { return RButtonDown; }

	bool GetWheelDown() { return WheelDown; }

private:
	//FSM
	StateMachine PlayerStateMachine;

	void InitalizeFSM();

	void FSM_Idle();
	void FSM_Attack();
	void FSM_Guard();
	void FSM_Dodge();
	void FSM_Parry();
	void FSM_Weak();
	void FSM_Stun();
	void FSM_GuardHit();
	void FSM_Stagger();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ULegacyCameraShake> HitCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ULegacyCameraShake> BlockCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ULegacyCameraShake> ParryCameraShake;

public:
	void FSM_PlayerChangeState(ECharacterState _State);

	virtual void Execution_Attacker(int32 _index) override;

	virtual void OnEnded_Attacker(UAnimMontage* _VictimMontage, bool _Interrupt) override;

	virtual void ClearDataAll() override;

	void SetTiredState(bool _bValue);

	void InitalizeExecutionData();

	bool IsPlayingExecution = false;

	bool EliteExecution = false;

	bool IsContinueExecution;

	bool RollDodge = false;

	bool DisableDodge = false;

	bool GetLockOn() { return IsLockOnIngaged; }


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExecutionMoveTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExecutionFindDist = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EliteExecutionHeal_HP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinionExecutionHeal_HP = 10.f;

	TMap<FString, FExecutionData*> ExecutionDatas;
	TArray<FString> BossExecutionMontages;
	TArray<FString> EliteExecutionMontages;
	TArray<FString> NormalExecutionMontages;
	TArray<FString> NormalExecutionStartMontages;

	FExecutionData* CurrentExecutionData;
	FExecutionData* BeforeExecutionData;

	FDoOnce ExecutionTargetUpdateOnce;

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecutionTargetUpdated(ABaseCharacterClass* _ExecutionTarget);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecutionStarted(FExecutionData _Data);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExecutionEnded();

	UFUNCTION(BlueprintImplementableEvent)
	void SetVisibleHPWidget(AActor* _target, bool _bVisible);

protected:

	virtual UAnimMontage* LowDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f) override;

	UAnimMontage* LowDamageReaction_Minion(AActor* _DamageCauser, int _Direction);

	virtual UAnimMontage* GuardBreakReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f) override;

	virtual UAnimMontage* HighDamageReaction(AActor* _DamageCauser, int _Direction, float _PlaySecond = -1.f) override;

	virtual UAnimMontage* KnockBackReaction(AActor* _DamageCauser, FVector _DirVector) override;

	virtual UAnimMontage* GuardReaction(AActor* _DamageCauser, ECombatDirection _Direction) override;

	virtual UAnimMontage* DeathReaction(AActor* _DamageCauser, int _Direction) override;

public:
	virtual UAnimMontage* GroggyReaction(AActor* _DamageCauser, int _Direction) override;

public:
	FOnHPChanged OnHpChanged;
	FOnStaminaChanged OnStaminaChanged;
	FOnStaminaRegenCancled OnStaminaRegenCancled;

	FOnNormalAttackStarted OnNormalAttackStarted;
	FOnCrushAttackStarted OnCrushAttackStarted;

	FOnGuardSuccessed OnGuardSuccessed;

	FOnHited OnHited;
	FOnLowHP OnLowHP;
	FOnEnoughHP OnEnoughHP;

	FOnTargetFinded OnTargetFinded;
	FOnTargetLosted OnTargetLosted;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerTired(bool value);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLockOnState(bool value, AActor* _target, bool _bPlaySound, bool _bEffectOnPlayer);
};