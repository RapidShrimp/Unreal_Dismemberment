// Fill out your copyright notice in the Description page of Project Settings.



#include "DismembermentSKMComponent.h"
#include "SkeletonDataAsset.h"


UDismembermentSKMComponent::UDismembermentSKMComponent()
{

}

void UDismembermentSKMComponent::InitialiseBones(USkeletonDataAsset* SkeletonData)
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
			return RootBones.IndexOfByKey(CurrentBone);		
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
	FMath::Clamp(Limbs[LimbIndex].CurrentHealth -= Damage,0,Limbs[LimbIndex].CurrentHealth);
	if(!Limbs[LimbIndex].CurrentHealth == 0)
		return;

	HideBoneByName(Limbs[LimbIndex].LimbRootName,PBO_Term);
	
	//Detach Limb Forever if MAX repairs Reached
	if(Limbs[LimbIndex].CurrentRepairs >= Limbs[LimbIndex].MaxRepairs)
	{
		OnLimbSevered.Broadcast(Limbs[LimbIndex]);
	}

	Limbs[LimbIndex].HasDetached = true;
	OnLimbRemoved.Broadcast(Limbs[LimbIndex]);
	
}

void UDismembermentSKMComponent::Handle_LimbRepair(int LimbIndex)
{
	if(!Limbs.IsValidIndex(LimbIndex) || Limbs[LimbIndex].HasDetached)
		return;

	RecreateSkeletalPhysics();
	Limbs[LimbIndex].CurrentHealth += 10;
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
		Limbs[i].CurrentHealth = Limbs[i].MaxHealth;
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