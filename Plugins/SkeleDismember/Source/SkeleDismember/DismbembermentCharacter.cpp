// Fill out your copyright notice in the Description page of Project Settings.


#include "DismbembermentCharacter.h"

#include "Components/CapsuleComponent.h"
#include "DismembermentSKMComponent.h"
#include "Engine/StaticMeshActor.h"

#include "SkeletonDataAsset.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "CableComponent/Classes/CableComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


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
	Mesh->SetRelativeRotation(FRotator{0,-90,0});
	
	//Particle Systems
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("Spawner",false);
	NiagaraComponent->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ADismbembermentCharacter::BeginPlay()
{
	Super::BeginPlay();
	Mesh->InitialiseBones(SkeletonData);
	Mesh->OnLimbRemoved.AddUniqueDynamic(this, &ADismbembermentCharacter::Handle_OnLimbRemoved);
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
	AStaticMeshActor* SpawnedMesh = SpawnMesh(BoneTrans.GetLocation(),GetActorRotation(),Limb);

	if(Limb.bUseTetherPhysics == true)
	{
	UE_LOG(LogTemp,Warning,TEXT("LimbRemoved"));
	SpawnPhysicsTether(SpawnedMesh,Limb);
	}
}

void ADismbembermentCharacter::Handle_OnLimbRepaired(FLimbGroupData Limb)
{
	Mesh->RepairLimb(Limb);
	//Todo Remove World Mesh
}

void ADismbembermentCharacter::SpawnPhysicsTether_Implementation(AStaticMeshActor* MeshToAttach, FLimbGroupData Limb)
{
	//Physics Collision Setup
	UStaticMeshComponent* MeshComp = MeshToAttach->GetStaticMeshComponent();
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionProfileName("Spectator");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_WorldStatic);
	MeshComp->SetSimulatePhysics(true);

	//Setup Cable Component Attachments
	FTransform SocketTransform = Mesh->GetSocketTransform(Limb.LimbRootName,RTS_World);
	UCableComponent* CableTether = NewObject<UCableComponent>(this,UCableComponent::StaticClass(),TEXT("CableComp"));

	FAttachmentTransformRules AttachmentRules = {EAttachmentRule::KeepRelative,true};
	AttachmentRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachmentRules.RotationRule = EAttachmentRule::KeepRelative;
	AttachmentRules.ScaleRule = EAttachmentRule::KeepRelative;

	if(CableTether)
	{
		CableTether->SetRelativeTransform(SocketTransform);
		CableTether->AttachToComponent(Mesh,AttachmentRules,Limb.LimbRootName);
		CableTether->EndLocation = {0,0,0};
		CableTether->CableWidth = SkeletonData->TetherWidth;
		CableTether->CableLength = SkeletonData->TetherLength;
		CableTether->SetAttachEndToComponent(MeshComp,"None");
		CableTether->RegisterComponent();
	}
	//Setup Physics Constraint
	UPhysicsConstraintComponent* PhysicsComp = NewObject<UPhysicsConstraintComponent>(this,UPhysicsConstraintComponent::StaticClass(),TEXT("PhysicsComp"));
	if(PhysicsComp)
	{
		SocketTransform.Rotator() = GetActorRotation();
		PhysicsComp->SetRelativeTransform(SocketTransform);
		PhysicsComp->AttachToComponent(Mesh,AttachmentRules,Limb.LimbRootName);
		PhysicsComp->SetConstrainedComponents(CapsuleComponent,"",MeshComp,"None");

		PhysicsComp->SetAngularSwing1Limit(ACM_Limited,20);
		PhysicsComp->SetAngularSwing2Limit(ACM_Limited,30);
		PhysicsComp->SetAngularTwistLimit(ACM_Free,50);

		PhysicsComp->SetLinearXLimit(LCM_Limited,10);
		PhysicsComp->SetLinearZLimit(LCM_Limited,SkeletonData->TetherLength-5.0f);
		PhysicsComp->RegisterComponent();
	}
}

void ADismbembermentCharacter::SpawnParticles_Implementation(FVector InLocation, FRotator InRotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(SkeletonData->ParticleSystem, NiagaraComponent, NAME_None, InLocation, InRotation, EAttachLocation::Type::KeepWorldPosition, true);
}
AStaticMeshActor* ADismbembermentCharacter::SpawnMesh_Implementation(FVector Location, FRotator Rotation,FLimbGroupData Limb)
{
	if(Limb.Mesh == nullptr)
		return nullptr;
	
	AStaticMeshActor* LimbMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	LimbMesh->SetMobility(EComponentMobility::Movable);
	LimbMesh->SetActorLocation(Location);
	UStaticMeshComponent* MeshComponent = LimbMesh->GetStaticMeshComponent();
	if(MeshComponent)
	{
		MeshComponent->SetStaticMesh(Limb.Mesh);
		MeshComponent->SetSimulatePhysics(true);
		
	}
	return LimbMesh;
}

void ADismbembermentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

