// Fill out your copyright notice in the Description page of Project Settings.

#include "HitEffectSoundComponent.h"
#include "ProjectTNH/Character/BaseCharacterClass.h"
#include "ProjectTNH/Weapon/WeaponBase.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values for this component's properties
UHitEffectSoundComponent::UHitEffectSoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UHitEffectSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UHitEffectSoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHitEffectSoundComponent::DoHitEffect(AActor* _Attacker, USkeletalMeshComponent* _SelfMesh)
{
	AttackerBase = Cast<ABaseCharacterClass>(_Attacker);
	if (!AttackerBase) return;

	AttackerWeaponBase = AttackerBase->MainWeapon;
	AttackerWeaponMesh = AttackerWeaponBase->GetItemMesh();
	AttackerMesh = AttackerBase->GetMesh();

	if (AttackerBase->pawnType == EPawnType::Player) SpawnSwordTrail();
}

void UHitEffectSoundComponent::SpawnSwordTrail()
{
	if (!AttackerWeaponMesh || !PlayerSwordTrail) return;

	UNiagaraComponent* NewNiagaraTrail = UNiagaraFunctionLibrary::SpawnSystemAttached(
		PlayerSwordTrail,
		AttackerWeaponMesh,
		TEXT("LongSwordHandSocket"),
		FVector(.0f, .0f, 20.0f),
		FRotator(.0f, .0f, .0f),
		EAttachLocation::KeepRelativeOffset,
		true, true
	);
}

void UHitEffectSoundComponent::SpawnHitBeam() {}

void UHitEffectSoundComponent::SpawnHitSound() {}

void UHitEffectSoundComponent::AddHitForce() {}

void UHitEffectSoundComponent::ClearAttacker(){}