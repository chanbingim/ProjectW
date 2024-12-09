// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponArmoryBase.h"
#include "Components/SphereComponent.h"
#include "ProjectTNH/Weapon/WeaponBase.h"
#include "ProjectTNH/Character/TNHCharacter.h"
#include "ProjectTNH/Components/CombatComponent.h"

// Sets default values
AWeaponArmoryBase::AWeaponArmoryBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Collider->SetupAttachment(RootComponent);
	Collider->SetSphereRadius(100.f);
}

// Called when the game starts or when spawned
void AWeaponArmoryBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponArmoryBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponArmoryBase::Interact_Implementation(AActor* Caller)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Interact With %s"), *Caller->GetName()));

	ATNHCharacter* Character = Cast<ATNHCharacter>(Caller);

	if (Character && SpawnWeaponClass)
	{
		if (IsValid(Character->GetCombatComponent()->GetMainWeapon()))
		{
			Character->GetCombatComponent()->GetMainWeapon()->Destroy();
			Character->GetCombatComponent()->SetMainWeapon(nullptr);
			Character->GetCombatComponent()->SetWeaponEquiped(false);
		}

		FActorSpawnParameters Params;

		Params.Instigator = Character;
		Params.Owner = Character;

		AWeaponBase* TempWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnWeaponClass, Caller->GetActorTransform(), Params);
		TempWeapon->SetEquipSocket(Character->GetCombatComponent()->GetWeaponSocket(TempWeapon, true));
		TempWeapon->SetUnEquipSocket(Character->GetCombatComponent()->GetWeaponSocket(TempWeapon, false));
		TempWeapon->OnInit();

		Character->GetCombatComponent()->SetMainWeapon(TempWeapon);
	}
}

