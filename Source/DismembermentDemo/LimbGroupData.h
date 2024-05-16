// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimbGroupData.generated.h"

/**
 * -
 */
USTRUCT(BlueprintType)
struct FLimbGroupData
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere)
 FName LimbRootName = "NULL";

 float LimbCurrentHealth = 40.0f;
 UPROPERTY(EditAnywhere)
 float LimbMaxHealth = LimbCurrentHealth;
 
 int CurrentRepairs = 0;
 UPROPERTY(EditAnywhere)
 int MaxRepairs = 1;
 
 UPROPERTY(VisibleAnywhere)
 bool HasDetached = false;
 
};