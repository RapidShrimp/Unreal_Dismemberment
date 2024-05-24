// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkeleDismember/Enums/LimbTypes.h"
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
 UPROPERTY(EditAnywhere, Category = "Dismemberment")
 TObjectPtr<UStaticMesh> Mesh;
 UPROPERTY(EditAnywhere, Category = "Dismemberment")
 E_LimbTypes LimbType = E_LimbTypes::None;
 
 UPROPERTY(EditAnywhere, Category = "Dismemberment")
 FName LimbRootName = "NULL";
 
 //Health
 float CurrentHealth = 40.0f;
 UPROPERTY(EditAnywhere, Category = "Dismemberment")
 float MaxHealth = CurrentHealth;
 int CurrentRepairs = 0;
 UPROPERTY(EditAnywhere, Category = "Dismemberment")
 int MaxRepairs = 1;

 //Physics
 UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
 bool HasDetached = false;
 UPROPERTY(EditDefaultsOnly, Category = "Dismemberment")
 bool bUseTetherPhysics;
 
};