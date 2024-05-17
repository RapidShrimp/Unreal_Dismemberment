// Fill out your copyright notice in the Description page of Project Settings.


#include "MyClass.h"

/*
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
*/
AMyClass::AMyClass()
{
}
