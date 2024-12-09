// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveDataStruct.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "SaveDatamanagerClass.generated.h"

class FJsonObject;

UCLASS(BlueprintAble, BlueprintType)
class PROJECTTNH_API USaveDatamanagerClass : public UObject
{
	GENERATED_BODY()
	
private :
	static USaveDatamanagerClass* Instance;

	UPROPERTY(VisibleAnywhere)
	FSaveData PlayerData;

	FVector ParseAsVector(TSharedPtr<FJsonObject> JasonValueObject, const FString KeyName);

public :
	UFUNCTION(BlueprintCallable, Category = "Singleton")
	static USaveDatamanagerClass* GetInstance();

	USaveDatamanagerClass();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void ShutDown();

	UFUNCTION(BlueprintCallable)
	void SavePlayerData();

	UFUNCTION(BlueprintCallable)
	void LoadPlayerData();

	UFUNCTION(BlueprintCallable)
	FSaveData GetPlayerData() { return PlayerData; }

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	bool bIsPlaycontinued = false;
};
