// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UWidgetComponent;

class UHPBar_Widget;

UENUM(BlueprintType)
enum class EAttackType
{
	EAT_LightAttack UMETA(DisplayName = "LightAttack"),
	EAT_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	EAT_BreakAttack UMETA(DisplayName = "EAT_BreakAttack"),
	EAT_None UMETA(DisplayNAme="None")
};

UENUM(BlueprintType)
enum class EDirection
{
	ED_Up UMETA(DisplayName = "Up"),
	ED_Left UMETA(DisplayName = "Left"),
	ED_Right UMETA(DisplayName = "Right"),
	ED_None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EPlayType : uint8
{
	EPT_Start UMETA(DisplayName = "Start"),
	EPT_Cancel UMETA(DisplayName = "Cancel"),
	EPT_Attack UMETA(DisplayName = "Attack"),
};

//UENUM(BlueprintType)
//enum class ELockOnType : uint8
//{
//	ELT_None UMETA(DisplayName = "None"),
//	ELT_SoftLockOn UMETA(DisplayName = "SoftLockOn"),
//	ELT_HardLockOn UMETA(DisplayName = "HardLockOn"),
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void EngageLockOn();

	void CheckLockOn();

	UFUNCTION(BlueprintCallable)
	void LockOnAction();

	UFUNCTION(BlueprintCallable)
	void DisEngageLockOn();

	UFUNCTION()
	void BreakLockOn();

	UFUNCTION()
	void HardLockOn();

	UFUNCTION()
	void SoftLockOn();

	void SetFocusEnemy(AActor* _Enemy);

	UFUNCTION()
	void SoftLockOnUpdate();

	UFUNCTION(BlueprintCallable)
	void SwitchLockedTarget(bool _bLeft = true);

	UFUNCTION(BlueprintCallable)
	void ToggleBattleMode();

	UFUNCTION(BlueprintCallable)
	void Interaction();

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void Charge(bool _bClicked);

	UFUNCTION(BlueprintCallable)
	void Dodge();

	UFUNCTION(BlueprintCallable)
	void Block(bool _bClicked);

	UFUNCTION(BlueprintCallable)
	void BreakAttack();

	UFUNCTION()
	void ComboStart();

	UFUNCTION(BlueprintCallable)
	void CheckNextCombo();

	UFUNCTION(BlueprintCallable)
	void ContinueCombo();

	UFUNCTION(BlueprintCallable)
	void EndDodge();

	UFUNCTION(BlueprintCallable)
	void StartCharge();

	UFUNCTION()
	void UpdateCharge(float _delta);

	UFUNCTION(BlueprintCallable)
	void EndCharge();

	UFUNCTION(BlueprintCallable)
	void ResetState(UAnimMontage* _Montage, bool _bInterrupt);

	UFUNCTION()
	FName GetWeaponSocket(class AWeaponBase* _Weapon, bool _bEquip);

	UFUNCTION()
	class UAnimMontage* GetWeaponEquipAnimation(class AWeaponBase* _Weapon, bool _bEquip);

	UFUNCTION()
	class UAnimMontage* GetWeaponAttackAnimation(class AWeaponBase* _Weapon, EDirection _Dir = EDirection::ED_None);

	UFUNCTION()
	TArray<class UAnimMontage*> GetWeaponAttackAnimationArray(class AWeaponBase* _Weapon, EDirection _Dir = EDirection::ED_None);

	UFUNCTION(BlueprintCallable)
	void InitOnWeaponEquip();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AWeaponBase* GetMainWeapon() { return MainWeapon; }

	UFUNCTION()
	FORCEINLINE void SetMainWeapon(class AWeaponBase* _Weapon) { MainWeapon = _Weapon; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetWeaponEquiped(bool _bEquiped) { WeaponEquiped = _bEquiped; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetWeqponEquiped() { return WeaponEquiped; }

	UFUNCTION()
	bool CanAttack();

	UFUNCTION()
	void OnSwordHit(FHitResult _LastItem);

	UFUNCTION(BlueprintCallable)
	void SetHPBar(UWidgetComponent* _HPBar);

	//UFUNCTION(BlueprintCallable)
	//void SetDirectionIndicator(class UDirectionIndicator_Widget* _DirectionIndicator);

	//UFUNCTION()
	//void DetectAttackDirection();

	UFUNCTION()
	void SetVisibilityHPBar(bool _Visible);

	UFUNCTION(BlueprintCallable)
	void SetAttackReaction(EReactionType _Reaction) { AttackReaction = _Reaction; };

	UFUNCTION(BlueprintCallable)
	void SetKnockbackVector(FVector _Amount) { KnockbackVector = _Amount; }


private:
	friend class ATNHCharacter;

	UPROPERTY()
	class ATNHCharacter* Character;

	UPROPERTY(VisibleAnywhere)
	class AWeaponBase* MainWeapon;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* HPBar;

	UPROPERTY(VisibleAnywhere)
	bool WeaponEquiped;

	UPROPERTY()
	TArray<AActor*> LockOnTargets;

	UPROPERTY(BlueprintReadOnly, Category = "Target", meta = (AllowPrivateAccess = "true"))
	class AActor* TargetActor;

	FTimerHandle BreakLockTimer;

	FTimerHandle SoftLockOnUpdateTimer;

	UPROPERTY(VisibleAnywhere)
	bool IsLockOnIngaged;

	//ELockOnType LockOnMode=ELockOnType::ELT_None;

	UPROPERTY(EditAnywhere)
	float SearchDistance = 1000.f;

	UPROPERTY()
	int32 CurrentComboIndex;

	UPROPERTY()
	EDirection CurrentComboDir;

	UPROPERTY()
	EDirection CurrentDirection;

	EReactionType AttackReaction;

	FVector KnockbackVector;

	UPROPERTY()
	bool IsAttacking = false;

	UPROPERTY()
	bool IsCheckingCombo = false;

	UPROPERTY()
	bool IsContinueCombo = false;

	UPROPERTY()
	EAttackType StoredNextAttack = EAttackType::EAT_None;

	UPROPERTY()
	bool IsChargeCombo = false;

	UPROPERTY()
	bool IsDodging = false;

	UPROPERTY()
	bool IsBlocking = false;

	UPROPERTY()
	bool IsCharging = false;

	UPROPERTY()
	float ChargingTime;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> SpawnWeaponClass;

private:
	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* LongSwordEquipAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* LongSwordUnEquipAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* GreatSwordEquipAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* GreatSwordUnEquipAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* LongSwordComboAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TArray<class UAnimMontage*> LongSwordUpComboAnims;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TArray<class UAnimMontage*> LongSwordLeftComboAnims;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TArray<class UAnimMontage*> LongSwordRightComboAnims;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TArray<class UAnimMontage*> DodgeAnims;

	UPROPERTY(EditAnywhere, Category="Animations")
	class UAnimMontage* BlockAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* BlockCounterAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* BreakAttackAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	class UAnimMontage* ChargingAttackAnim;

private:
	UFUNCTION(BlueprintCallable)
	void PlayEquipAnim();

	UFUNCTION(BlueprintCallable)
	void PlayUnEquipAnim();

	UFUNCTION(BlueprintCallable)
	void PlayAttackComboAnim(int32 _Index);

	UFUNCTION(BlueprintCallable)
	void PlayChargeComboAnim(int32 _Index);

	UFUNCTION(BlueprintCallable)
	void PlayDodgeAnim();

	UFUNCTION(BlueprintCallable)
	void PlayBlockAnim(bool _bOut = false);

	UFUNCTION(BlueprintCallable)
	void PlayBlockCounterAnim();

	UFUNCTION(BlueprintCallable)
	void PlayBreakAttackAnim();

	UFUNCTION(BlueprintCallable)
	void PlayChargingAttackAnim(EPlayType _PlayType=EPlayType::EPT_Start);
};
