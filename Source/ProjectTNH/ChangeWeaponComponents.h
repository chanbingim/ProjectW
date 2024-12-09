// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChangeWeaponComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTNH_API UChangeWeaponComponents : public UActorComponent
{
	GENERATED_BODY()
private:
	class AActor* OwnerClass = nullptr;

	AActor* SubWeapon = nullptr;

	FTimerHandle ChangeWeaponTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> ChangeWeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ChangeRange = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChangeAnimation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* MainWeaponToSubWeaponAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChangeAnimation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* SubWeaponToMainWeaponAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString SocketNameMainWeapon = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString SocketNameSubWeapon = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString SocketNameMainWeaponEquip = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FString SocketNameSubWeaponEquip = "";

public:	
	// Sets default values for this component's properties
	UChangeWeaponComponents();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	UFUNCTION(BlueprintCallable,Category = "ChangeWeapon")
	void AttachWeapon(AActor* _Item, FString ToSocket, EPawnType ChangeType, bool bIsSettingData);
	
	UFUNCTION(BlueprintCallable)
	void SetOwnerActor(AActor* Owner) { OwnerClass = Owner; }

	UFUNCTION(BlueprintCallable)
	AActor* GetSubWeapon() { return SubWeapon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
	bool bIsActiveComponents = false;
};
