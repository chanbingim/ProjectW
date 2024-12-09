// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Character/BaseCharacterClass.h"
#include "../Ai_Montage/AiMontage_Interface.h"
#include "Ai_CharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnActionDelegate);

UCLASS()
class PROJECTTNH_API AAi_CharacterBase : public ABaseCharacterClass, public IAiMontage_Interface
{
	GENERATED_BODY()

private:
	FTimerHandle AttackCoolTime;
	FTimerHandle PostGroggyAttackDelay;
	float CoolTime;
	
	UPROPERTY()
	float CurrentCapsuleHitTime = 0.0f;

	UPROPERTY()
	float UpdateRotationTime = 0.0f;

	UPROPERTY()
	FRotator TargetYaw;

	UPROPERTY()
	class AAiBaseController* Ai_Controller;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void ClearDataAll() override;
	virtual UAnimMontage* DeathReaction(AActor* _DamageCauser, int _Direction) override;
	virtual UAnimMontage* GroggyReaction(AActor* _DamageCauser, int _Direction) override;
	virtual void OnEnded_ParryStunReaction(UAnimMontage* _HitReaction, bool _Interrupt) override;
	virtual void Execution_Victim(AActor* _ExecutionAttacker, UAnimMontage* _VictimMontage, float _BetweenDistance);
	virtual void OnEnded_Victim(UAnimMontage* _VictimMontage, bool _Interrupt) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	class UAttackListComponents* _AttackListComponents = nullptr;

	UPROPERTY(EditAnywhere)
	class UAiMoveComponents* _MoveComponents = nullptr;
public:
	// Sets default values for this character's properties
	AAi_CharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void InitData() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FString AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Push_Reaction")
	bool Enhanced_Push = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dealy", meta = (Tooltip = " AI에 플레이어가 충돌한 시간"))
	float CapsuleHitTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dealy", meta = (Tooltip = "그로기 이후 공격을 하기까지 딜레이시간"))
	float PostGroggyDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeaponBase> SubWeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDecalComponent* DecalComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AWeaponBase* subWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiredState", meta = (Tooltip = "0.0 ~ 1.0f Betwwen Data"))
	float LowBalanceWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance", meta = (Tooltip = "타겟과 유지할 최대 거리"))
	float SafeDistance = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TiredState", meta = (Tooltip = "체력이 얼마 아래부터 보정을 시작할건가 0 ~ 1 Data"))
	float LowBalancePercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "AI 반응 시간"))
	float reactionTime = 0.0f;

	UFUNCTION()
	void IAttackEnd();

	UFUNCTION()
	virtual void IHitEnd();

	FTimerHandle GetAttackTimer() { return AttackCoolTime; }
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnParryHited_Func(AActor* _ParryCauser, float _BalanceDamage) override;

	UFUNCTION()
	void SettingLeftWeapon();

	UFUNCTION()
	void ResetData(float _value);

	UFUNCTION(BlueprintCallable)
	void AttackSelect(int Index, AActor* Target);

	UFUNCTION()
	void AIAttackToPlayer(FAttackRow Data);

	UFUNCTION()
	void DeathRemoveComponents();

	UFUNCTION(BlueprintCallable)
	void AddObjectPoolAndCommander();

	UFUNCTION()
	void ResetTimer();


	UFUNCTION(BlueprintCallable)
	UAttackListComponents* GetAttackListComponents() { return _AttackListComponents; }

	UFUNCTION(BlueprintCallable)
	UAiMoveComponents* GetMoveComponents() { return _MoveComponents; }

	FOnActionDelegate FOnAttackEnd;
	FOnActionDelegate FOnGuardEnd;
	FOnActionDelegate FOnMoveDel;
};
