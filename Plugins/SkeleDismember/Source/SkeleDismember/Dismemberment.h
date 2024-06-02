// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/LimbGroupData.h"
#include "Dismemberment.generated.h"

class UCableComponent;
class UPhysicsConstraintComponent;
class USphereComponent;
class AStaticMeshActor;
struct FLimbGroupData;
class UNiagaraComponent;
class USkeletonDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbRemoved,FLimbGroupData,Limb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbSevered,FLimbGroupData,Limb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLimbRepaired,FLimbGroupData,Limb);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnParticles,FVector, InLocation, FRotator, InRotation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLimbSpawned,FVector,Location, FRotator,Rotation,FLimbGroupData,Limb);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKELEDISMEMBER_API UDismemberment : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDismemberment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//Delegates
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbRemoved OnLimbRemoved;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbSevered OnLimbSevered;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbRepaired OnLimbRepaired;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnSpawnParticles OnSpawnParticles;
	
	UPROPERTY(BlueprintAssignable,Category = "Dismemberment")
	FOnLimbSpawned OnLimbSpawned;
	
	//Data
	TArray<TObjectPtr<UPhysicsConstraintComponent>> PhysComponents;
	TArray<TObjectPtr<UCableComponent>> CableComponents;
	TArray<TObjectPtr<AStaticMeshActor>> Meshes;
	
	UPROPERTY(VisibleAnywhere, Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Dismemberment")
	TObjectPtr<USkeletalMeshComponent> SkeleMesh;
	UPROPERTY(EditAnywhere, Category = "Dismemberment")
	TObjectPtr<USkeletonDataAsset> SkeletonData;

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void InitialiseBones();
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void Handle_LimbHit(FName HitBoneName, float Damage);
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbSevered(FLimbGroupData Limb);
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbRemoved(FLimbGroupData Limb);
	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbRepaired(FLimbGroupData Limb);
	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void RepairAllLimbs();
	void RecreateSkeletalPhysics();

	
	//Limb Visuals
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	AStaticMeshActor* SpawnMesh(FVector Location, FRotator Rotation, FLimbGroupData Limb);
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	UNiagaraComponent* SpawnParticles(FVector InLocation, FRotator InRotation);

	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	UPhysicsConstraintComponent* SpawnPhysicsTether(AStaticMeshActor* MeshToAttach,FLimbGroupData Limb);
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	UCableComponent* SpawnCableTether(AStaticMeshActor* MeshToAttach, FName SocketName);

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	int GetLimbIndexFromBoneName(FName Bone);
	void UpdateLimbRefs(int LimbIndex, AStaticMeshActor* Mesh, UPhysicsConstraintComponent* Phys, UCableComponent* Cable);
	void DeleteLimbRefs(int LimbIndex);
	
	FTransform GetLimbTransform(FName BoneName) {return SkeleMesh->GetBoneTransform(SkeleMesh->GetBoneIndex(BoneName));}
	FTransform GetBoneParentTransform(FName BoneName) {return GetLimbTransform(SkeleMesh->GetParentBone(BoneName));}

	UFUNCTION(BlueprintCallable,Category = "Dismemberment")
	void EvaluateLimbs(int& Arms, int& Legs, int& Heads, int& Other);

};
