// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/LimbGroupData.h"
#include "DismembermentSKMComponent.generated.h"

class USkeletonDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbRemoved,FLimbGroupData, Limb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbSevered,FLimbGroupData, Limb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbHealed,FLimbGroupData,Limb);
/**
 * 
 */
UCLASS()
class DISMEMBERMENTDEMO_API UDismembermentSKMComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UDismembermentSKMComponent();

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void InitialiseBones(USkeletonDataAsset* SkeletonData);
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	int GetLimbIndexFromBoneName(FName Bone);
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbHit(FName HitBoneName, float Damage);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbRepair(int LimbIndex);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void RepairAllLimbs();

	void RecreateSkeletalPhysics();

public:
	FTransform GetLimbTransform(FName BoneName) {return GetBoneTransform(GetBoneIndex(BoneName));}
	FTransform GetBoneParentTransform(FName BoneName) {return GetLimbTransform(GetParentBone(BoneName));}
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbRemoved OnLimbRemoved;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbSevered OnLimbSevered;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbHealed OnLimbHealed;

	
};
