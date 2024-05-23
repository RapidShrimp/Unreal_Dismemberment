// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DismembermentDemo/Enums/LimbTypes.h"
#include "UObject/Object.h"
#include "LimbGroupData.generated.h"

/**
 * -
 */
USTRUCT(BlueprintType)
struct FLimbGroupData
{
 GENERATED_BODY()

 //Asset Visual & Audio Data
 UPROPERTY(EditAnywhere)
 TObjectPtr<UStaticMesh> Mesh;
 UPROPERTY(EditAnywhere)
 E_LimbTypes LimbType = E_LimbTypes::None;
 
 UPROPERTY(EditAnywhere)
 FName LimbRootName = "NULL";
 
 //Health
 float CurrentHealth = 40.0f;
 UPROPERTY(EditAnywhere)
 float MaxHealth = CurrentHealth;
 int CurrentRepairs = 0;
 UPROPERTY(EditAnywhere)
 int MaxRepairs = 1;

 //Physics
 UPROPERTY(VisibleAnywhere)
 bool HasDetached = false;
 UPROPERTY(EditDefaultsOnly)
 bool bUseTetherPhysics;
 
};