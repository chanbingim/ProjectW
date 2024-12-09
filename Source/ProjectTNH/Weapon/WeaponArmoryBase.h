// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectTNH/Interfaces/InteractableInterface.h"
#include "WeaponArmoryBase.generated.h"

UCLASS()
class PROJECTTNH_API AWeaponArmoryBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponArmoryBase();

private:
	USceneComponent* RootComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* Collider;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponBase> SpawnWeaponClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void Interact_Implementation(AActor* Caller) override;
};
