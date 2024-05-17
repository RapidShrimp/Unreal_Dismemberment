// Fill out your copyright notice in the Description page of Project Settings.


#include "DismembermentSKMComponent.h"

#include "SkeletonDataAsset.h"
#include "Structs/LimbGroupData.h"

void UDismembermentSKMComponent::InitialiseBones()
{
	if(!SkeletonData)
		return;
	SetSkeletalMeshAsset(SkeletonData->SkeletalMesh);
	Limbs = SkeletonData->Limbs;
}

void UDismembermentSKMComponent::FillSuggestedBoneNames()
{
	TArray<FName> BoneNames;
	GetBoneNames(BoneNames);

	FLimbGroupData FoundLimb;
	for (FName Bone : BoneNames) //Check ALL Bones Against Substrings, Add them if a substring matches
	{
		bool bIsValid = false;

		for (FString ValidName : ValidNameSubstrings)
			if(Bone.ToString().ToLower().Contains(ValidName) )
			{
				bIsValid = true;
			}
		for (FString InvalidSubstring : InvalidNameSubstrings)
			if(Bone.ToString().ToLower().Contains(InvalidSubstring))
			{
				bIsValid = false;
			}

		if(bIsValid)
		{
			FoundLimb.LimbRootName = Bone;
			FoundLimb.HasDetached = false;
			FoundLimb.LimbCurrentHealth = 50;
			FoundLimb.LimbMaxHealth = 50;
			Limbs.Add(FoundLimb);
		}
	}
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
	
	//TODO Spawn Mesh of limb
	
	//Detach Limb Forever OR Detach for Repair
	if(Limbs[LimbIndex].CurrentRepairs >= Limbs[LimbIndex].MaxRepairs)
	{
		HideBoneByName(Limbs[LimbIndex].LimbRootName,PBO_Term);
		Limbs[LimbIndex].HasDetached = true;
		UE_LOG(LogTemp,Error,TEXT("LIMB IS SEVERED"));
	}
	else
	{
		HideBoneByName(Limbs[LimbIndex].LimbRootName,PBO_None);
		FBoneTransform NewTransform;
		NewTransform.Transform.SetLocation(FVector{0,0,0});
	}
	UE_LOG(LogTemp,Warning,TEXT("%s"), *GetBoneLocation(Limbs[LimbIndex].LimbRootName).ToString());
}

void UDismembermentSKMComponent::Handle_LimbRepair(int LimbIndex)
{
	if(!Limbs.IsValidIndex(LimbIndex) || Limbs[LimbIndex].HasDetached)
		return;
	
	Limbs[LimbIndex].LimbCurrentHealth += 10;
	Limbs[LimbIndex].CurrentRepairs += 1;
	Limbs[LimbIndex].HasDetached = false;
	
	UnHideBoneByName(Limbs[0].LimbRootName);

	//TODO Physics Needs to be Rebuilt
	
}

