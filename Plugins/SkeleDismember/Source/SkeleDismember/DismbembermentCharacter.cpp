// Fill out your copyright notice in the Description page of Project Settings.


#include "DismbembermentCharacter.h"

#include "BlendSpaceAnalysis.h"
#include "NiagaraFunctionLibrary.h"
#include "DismembermentSKMComponent.h"
#include "NiagaraComponent.h"
#include "SkeletonDataAsset.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/PhysicsAsset.h"


class AStaticMeshActor;
// Sets default values
ADismbembermentCharacter::ADismbembermentCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"),false);
	CapsuleComponent-> SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->SetCapsuleHalfHeight(96);
	CapsuleComponent->SetCapsuleRadius(45);
	SetRootComponent(CapsuleComponent);

	//Mesh Component
	Mesh = CreateDefaultSubobject<UDismembermentSKMComponent>(TEXT("Dismemberment SKM Comp"),false);
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName(TEXT("CharacterMesh"));
	Mesh->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetRelativeLocation(FVector{0,0,-96});
	Mesh->SetRelativeRotation(FRotator{0,0,0});
	
	//Particle Systems
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("Spawner",false);
	NiagaraComponent->SetupAttachment(Mesh);
	
}

// Called when the game starts or when spawned
void ADismbembermentCharacter::BeginPlay()
{
	Super::BeginPlay();
	Mesh->InitialiseBones(SkeletonData);
	Mesh->OnLimbRemoved.AddUniqueDynamic(this, &ADismbembermentCharacter::Handle_OnLimbSevered);
	Mesh->OnLimbSevered.AddUniqueDynamic(this, &ADismbembermentCharacter::Handle_OnLimbSevered);
}

void ADismbembermentCharacter::Handle_OnLimbSevered(FLimbGroupData Limb)
{
	FTransform BoneTrans = Mesh->GetLimbTransform(Limb.LimbRootName);
	FVector BoneDir = BoneTrans.GetLocation() - Mesh->GetBoneParentTransform(Limb.LimbRootName).GetLocation();
	FRotator BoneRot = FRotationMatrix::MakeFromX(BoneDir).Rotator();
	
	SpawnParticles(BoneTrans.GetLocation(),BoneRot);
	SpawnMesh(BoneTrans.GetLocation(),GetActorRotation(),Limb);
}

void ADismbembermentCharacter::Handle_OnLimbRemoved(FLimbGroupData Limb)
{
	FTransform BoneTrans = Mesh->GetLimbTransform(Limb.LimbRootName);
	FVector BoneDir = BoneTrans.GetLocation() - Mesh->GetBoneParentTransform(Limb.LimbRootName).GetLocation();
	FRotator BoneRot = FRotationMatrix::MakeFromX(BoneDir).Rotator();
	SpawnParticles(BoneTrans.GetLocation(),BoneRot);
	SpawnMesh(BoneTrans.GetLocation(),GetActorRotation(),Limb);
}

void ADismbembermentCharacter::Handle_OnLimbRepaired(FLimbGroupData Limb)
{
	Mesh->RepairLimb(Limb);
}

void ADismbembermentCharacter::SpawnParticles_Implementation(FVector InLocation, FRotator InRotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(SkeletonData->ParticleSystem, NiagaraComponent, NAME_None, InLocation, InRotation, EAttachLocation::Type::KeepWorldPosition, true);
}
void ADismbembermentCharacter::SpawnMesh_Implementation(FVector Location, FRotator Rotation,FLimbGroupData Limb)
{
	if(Limb.Mesh == nullptr)
		return;
	
	AStaticMeshActor* LimbMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	LimbMesh->SetMobility(EComponentMobility::Movable);
	LimbMesh->SetActorLocation(Location);
	UStaticMeshComponent* MeshComponent = LimbMesh->GetStaticMeshComponent();
	if(MeshComponent)
	{
		MeshComponent->SetStaticMesh(Limb.Mesh);
		MeshComponent->SetSimulatePhysics(true);
	}
	
}

void ADismbembermentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

