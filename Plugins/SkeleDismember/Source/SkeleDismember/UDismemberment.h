// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/LimbGroupData.h"
#include "UDismemberment.generated.h"

class USphereComponent;
class AStaticMeshActor;
struct FLimbGroupData;
class UNiagaraComponent;
class USkeletonDataAsset;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKELEDISMEMBER_API UUDismemberment : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUDismemberment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Dismemberment")
	TObjectPtr<USkeletalMeshComponent> SkeleMesh;
	UPROPERTY(EditAnywhere, Category = "Dismemberment")
	TObjectPtr<USkeletonDataAsset> SkeletonData;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
	
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbSevered(FLimbGroupData Limb);
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbRemoved(FLimbGroupData Limb);
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbRepaired(FLimbGroupData Limb);

	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	void SpawnPhysicsTether(AStaticMeshActor* MeshToAttach,FLimbGroupData Limb);
	
	//LimbVisuals
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	void SpawnParticles(FVector InLocation, FRotator InRotation);
	
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	AStaticMeshActor* SpawnMesh(FVector Location, FRotator Rotation, FLimbGroupData Limb);

//USKELETALMESHDSKM


	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void InitialiseBones();
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	int GetLimbIndexFromBoneName(FName Bone);
	
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbHit(FName HitBoneName, float Damage);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void RepairAllLimbs();

	void RecreateSkeletalPhysics();
	
	FTransform GetLimbTransform(FName BoneName) {return SkeleMesh->GetBoneTransform(SkeleMesh->GetBoneIndex(BoneName));}
	FTransform GetBoneParentTransform(FName BoneName) {return GetLimbTransform(SkeleMesh->GetParentBone(BoneName));}

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void EvaluateLimbs();


};
