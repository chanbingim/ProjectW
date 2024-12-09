// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"

#include "HitEffectSoundComponent.generated.h"

UCLASS(ClassGroup = (Component), meta = (BlueprintSpawnableComponent))
class PROJECTTNH_API UHitEffectSoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHitEffectSoundComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void DoHitEffect(AActor* _Attacker, USkeletalMeshComponent* _SelfMesh);

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void SpawnSwordTrail();

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void SpawnHitBeam();

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void SpawnHitSound();

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void AddHitForce();

	UFUNCTION(BlueprintCallable, Category = "HitEffectComponent")
	void ClearAttacker();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Naiagara")
	class UNiagaraSystem* PlayerSwordTrail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Naiagara")
	class UNiagaraSystem* HitBeam;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Naiagara")
	class UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundCue* HittedVoice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundCue* HittedSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundAttenuation* VoiceAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundAttenuation* HittedAttenuation;

private:
	UPROPERTY()
	class ABaseCharacterClass* AttackerBase;

	UPROPERTY()
	class AWeaponBase* AttackerWeaponBase;

	UPROPERTY()
	class UMeshComponent* AttackerWeaponMesh;

	UPROPERTY()
	class USkeletalMeshComponent* AttackerMesh;
};
