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
class DISMEMBERMENTDEMO_API USkeletonDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Dismemberment")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Dismemberment")
	TObjectPtr<UNiagaraSystem> ParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
};
