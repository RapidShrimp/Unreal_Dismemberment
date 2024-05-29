// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/LimbGroupData.h"
#include "NiagaraComponent.h"
#include "SkeletonDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class SKELEDISMEMBER_API USkeletonDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UNiagaraSystem> ParticleSystem;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Dismemberment")
	float TetherLength = 20.0f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Dismemberment")
	float TetherWidth = 4.5f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
};
