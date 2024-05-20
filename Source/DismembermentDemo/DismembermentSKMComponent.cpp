// Fill out your copyright notice in the Description page of Project Settings.



#include "DismembermentSKMComponent.h"
#include "NiagaraComponent.h"
#include "SkeletonDataAsset.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Structs/LimbGroupData.h"


UDismembermentSKMComponent::UDismembermentSKMComponent()
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("Spawner",false);
	NiagaraComponent->SetupAttachment(GetAttachmentRoot());
}

void UDismembermentSKMComponent::InitialiseBones()
{
	if(!SkeletonData)
		return;
	SetSkeletalMeshAsset(SkeletonData->SkeletalMesh);
	Limbs = SkeletonData->Limbs;
}

int UDismembermentSKMComponent::GetLimbIndexFromBoneName(FName Bone)
{
	//Get All Root Bones
	FName CurrentBone = Bone;
	TArray<FName> RootBones;
	for (FLimbGroupData LimbData : Limbs)
		RootBones.Add(LimbData.LimbRootName);
	
	do //Check if Hit Bone is Connected to a Limb
	{
		if(RootBones.Contains(CurrentBone))
		{
			UE_LOG(LogTemp,Display,TEXT("%s"),*CurrentBone.ToString());
			return RootBones.IndexOfByKey(CurrentBone);		
		}
		CurrentBone = GetParentBone(CurrentBone);
	} while(CurrentBone != "none");

	return -1;
}

void UDismembermentSKMComponent::Handle_LimbHit(FName HitBoneName, float Damage)
{
	int LimbIndex = GetLimbIndexFromBoneName(HitBoneName);
	
	if(LimbIndex == -1 || Limbs[LimbIndex].HasDetached)
		return;
	
	//Take Damage to the Limb & Check if its hit 0
	FMath::Clamp(Limbs[LimbIndex].LimbCurrentHealth -= Damage,0,1000);
	if(!Limbs[LimbIndex].LimbCurrentHealth == 0)
		return;


	HideBoneByName(Limbs[LimbIndex].LimbRootName,PBO_Term);
	
	//Detach Limb Forever if MAX repairs Reached
	if(Limbs[LimbIndex].CurrentRepairs >= Limbs[LimbIndex].MaxRepairs)
		Limbs[LimbIndex].HasDetached = true;

	
	FTransform BoneTrans = GetBoneTransform(GetBoneIndex(Limbs[LimbIndex].LimbRootName));

	//Spawn Mesh
	AStaticMeshActor* LimbMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	LimbMesh->SetMobility(EComponentMobility::Movable);
	LimbMesh->SetActorLocation(BoneTrans.GetLocation());
	UStaticMeshComponent* MeshComponent = LimbMesh->GetStaticMeshComponent();
	if(MeshComponent)
	{
		MeshComponent->SetStaticMesh(Limbs[LimbIndex].LimbMesh);
		MeshComponent->SetSimulatePhysics(true);
	}
	
	//Todo Spawn Particles
	SpawnParticles(BoneTrans);

	//Todo Setup Tether Constraints

	//Todo Rope Renderer
	
}

void UDismembermentSKMComponent::Handle_LimbRepair(int LimbIndex)
{
	if(!Limbs.IsValidIndex(LimbIndex) || Limbs[LimbIndex].HasDetached)
		return;

	RecreateSkeletalPhysics();
	Limbs[LimbIndex].LimbCurrentHealth += 10;
	Limbs[LimbIndex].CurrentRepairs += 1;
	UnHideBoneByName(Limbs[LimbIndex].LimbRootName);
	for (FLimbGroupData Limb : Limbs)
	{
		if(IsBoneHidden(GetBoneIndex(Limb.LimbRootName)))
			TermBodiesBelow(Limb.LimbRootName);
	}
}

void UDismembermentSKMComponent::RepairAllLimbs()
{
	for (int i = 0; i < Limbs.Num();i++)
	{
		Limbs[i].CurrentRepairs = 0;
		Limbs[i].HasDetached = false;
		Limbs[i].LimbCurrentHealth = Limbs[i].LimbMaxHealth;
		UnHideBoneByName(Limbs[i].LimbRootName);
	}
	RecreateSkeletalPhysics();
}

void UDismembermentSKMComponent::RecreateSkeletalPhysics()
{
	TermArticulated();
	InitArticulated(GetWorld()->GetPhysicsScene());
	UE_LOG(LogTemp,Warning,TEXT("Rebuilt Skeletal Physics for %s"),*GetOwner()->GetName());
}

void UDismembermentSKMComponent::SpawnParticles(FTransform EmitterTransform)
{
	OnSpawnParticles.Broadcast(EmitterTransform);
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(SkeletonData->ParticleSystem, NiagaraComponent, NAME_None, EmitterTransform.GetLocation(), EmitterTransform.Rotator(), EAttachLocation::Type::KeepRelativeOffset, true);
	UE_LOG(LogTemp,Warning,TEXT("%s"), *EmitterTransform.ToString());
}