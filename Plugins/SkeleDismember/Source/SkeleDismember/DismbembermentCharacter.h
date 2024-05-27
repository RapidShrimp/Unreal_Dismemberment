// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DismbembermentCharacter.generated.h"

struct FLimbGroupData;
class UNiagaraComponent;
class USkeletonDataAsset;
class UDismembermentSKMComponent;

UCLASS()
class SKELEDISMEMBER_API ADismbembermentCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADismbembermentCharacter();

protected:
	virtual void BeginPlay() override;
	
	//Component Defaults

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Dismemberment")
	TObjectPtr<UDismembermentSKMComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Dismemberment")
	TObjectPtr<USkeletonDataAsset> SkeletonData;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	//Particle Systems
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Dismemberment")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UFUNCTION(Category = "Dismemberment")
	void Handle_OnLimbSevered(FLimbGroupData Limb);
	
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	void SpawnParticles(FVector InLocation, FRotator InRotation);

	//LimbVisuals
	UFUNCTION(BlueprintNativeEvent,Category = "Dismemberment")
	void SpawnMesh(FVector Location, FRotator Rotation, FLimbGroupData Limb);
};
