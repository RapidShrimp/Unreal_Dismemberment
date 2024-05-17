// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "DismembermentSKMComponent.generated.h"

class UNiagaraComponent;
class USkeletonDataAsset;
struct FLimbGroupData;
/**
 * 
 */
UCLASS()
class DISMEMBERMENTDEMO_API UDismembermentSKMComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	UDismembermentSKMComponent();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	
	UPROPERTY(EditAnywhere, Category = "Dismemberment")
	TObjectPtr<USkeletonDataAsset> SkeletonData;

	UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
	
	UFUNCTION(BlueprintCallable)
	void InitialiseBones();

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	int GetLimbIndexFromBoneName(FName Bone);
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbHit(FName HitBoneName, float Damage);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbRepair(int LimbIndex);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void SpawnParticlesAtLocation(FVector Location);
};
