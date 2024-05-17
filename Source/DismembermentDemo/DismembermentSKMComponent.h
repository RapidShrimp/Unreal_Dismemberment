// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "DismembermentSKMComponent.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
	TArray<FString> ValidNameSubstrings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
	TArray<FString> InvalidNameSubstrings;
	
	UPROPERTY(EditAnywhere, Category = "Dismemberment")
	TObjectPtr<USkeletonDataAsset> SkeletonData;

	UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;

	UFUNCTION(BlueprintCallable)
	void InitialiseBones();
	
	UFUNCTION(BlueprintCallable, Category = "Dismemberment")
	void FillSuggestedBoneNames();

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	int GetLimbIndexFromBoneName(FName Bone);
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbHit(FName HitBoneName, float Damage);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbRepair(int LimbIndex);
};
