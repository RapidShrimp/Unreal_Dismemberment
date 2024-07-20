// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/LimbGroupData.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "SkeletonDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class SKELEDISMEMBER_API USkeletonDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * Skeletal Mesh of the Mannequin that spawns
	 * Note: **This Will Override the skeletal mesh on the actor**
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	/**
	 * CableComponent Material
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UMaterialInstance> CableMaterial;

	/**
	* Particle System that will spawn on the detach socket
	* Note: **This needs to be a particle function with an end (Not Infinite)**
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UNiagaraSystem> ParticleSystem;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Dismemberment")
	float TetherLength = 20.0f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Dismemberment")
	float TetherWidth = 4.5f;

	/**
	 * Override The Leg Count of data asset
	 * (Use if you need legs a leg count but dont have leg dismemberment enabled)
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	int OverrideLegs = 0;
	/**
	* Override The Arm Count of data asset
	 * (Use if you need legs a leg count but dont have leg dismemberment enabled)
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	int OverrideArms = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
};
