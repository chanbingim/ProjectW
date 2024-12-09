// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "ProjectTNH/Character/TNHCharacter.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->SetupAttachment(RootComponent);
	WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->SetupAttachment(RootComponent);
	WeaponStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMeshComponent* AWeaponBase::GetItemMesh()
{
	if (IsValid(WeaponSkeletalMesh) && IsValid(WeaponSkeletalMesh->GetSkeletalMeshAsset()))
	{
		return WeaponSkeletalMesh;
	}
	else if (IsValid(WeaponStaticMesh) && IsValid(WeaponStaticMesh->GetStaticMesh()))
	{
		return WeaponStaticMesh;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ItemMeshError")));

	return nullptr;
}

void AWeaponBase::OnInit()
{
	OwnerCharacter = (OwnerCharacter == nullptr) ? Cast<AALSCharacter>(GetOwner()) : OwnerCharacter;

	OnUnEquipped();
}

void AWeaponBase::OnEquipped()
{
	Equipped = true;

	AttachWeapon(EquipSocketName);
}

void AWeaponBase::OnUnEquipped()
{
	Equipped = false;

	AttachWeapon(UnEquipSocketName);
}

void AWeaponBase::AttachWeapon(FName _AttachSocket)
{
    if (IsValid(OwnerCharacter))
    {
        ATNHCharacter* _PlayerCharacter = Cast<ATNHCharacter>(OwnerCharacter);
        if (_PlayerCharacter)
        {
			USkeletalMeshComponent* BotBodyComponent = _PlayerCharacter->BaseBody;
			if (IsValid(BotBodyComponent))
            {
                
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Attach Complete")));
                AttachToComponent(BotBodyComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, _AttachSocket);
            }
        }
        else if (IsValid(OwnerCharacter->GetMesh()))
        {
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Attach to OwnerCharacter Mesh")));
            AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, _AttachSocket);
        }
        else
        {
           // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("OwnerCharacter cast to ATNHCharacter failed")));
        }
    }
    else
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("OwnerCharacter is invalid")));
    }
}