// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectTNH/Weapon/WeaponTypes.h"
#include "WeaponBase.generated.h"

UCLASS()
class PROJECTTNH_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

private:
	USceneComponent* RootComponent;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WeaponSkeletalMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponStaticMesh;

	bool Equipped = false;

	class AALSCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere)
	FName EquipSocketName;

	UPROPERTY(EditAnywhere)
	FName UnEquipSocketName;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	UMeshComponent* GetItemMesh();

	UFUNCTION()
	void OnInit();

	UFUNCTION(BlueprintCallable)
	void OnEquipped();

	UFUNCTION(BlueprintCallable)
	void OnUnEquipped();

	UFUNCTION(BlueprintCallable)
	void AttachWeapon(FName _AttachSocket);

public:
	UFUNCTION()
	FORCEINLINE bool IsEquipped() { return Equipped; }

	UFUNCTION()
	FORCEINLINE void SetEquipSocket(FName _AttachSocket) { EquipSocketName = _AttachSocket; }

	UFUNCTION()
	FORCEINLINE void SetUnEquipSocket(FName _AttachSocket) { UnEquipSocketName = _AttachSocket; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }
};
