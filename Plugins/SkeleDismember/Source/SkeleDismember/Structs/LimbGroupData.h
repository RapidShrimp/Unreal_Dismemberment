// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
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
 UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 TObjectPtr<UStaticMesh> Mesh;
 UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 E_LimbTypes LimbType = E_LimbTypes::None;
 
 UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 FName LimbRootName = "NULL";
 
 //Health
 UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 float CurrentHealth = 40.0f;
 UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 float MaxHealth = CurrentHealth;
 UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 int CurrentRepairs = 0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 int MaxRepairs = 1;

 //Physics
 UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
 bool HasDetached = false;
 UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Dismemberment")
 bool bUseTetherPhysics;
 
};