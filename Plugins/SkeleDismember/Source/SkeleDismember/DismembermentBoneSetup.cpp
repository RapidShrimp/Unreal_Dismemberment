// Fill out your copyright notice in the Description page of Project Settings.


#include "DismembermentBoneSetup.h"
#define DEBUG(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

UDismembermentBoneSetup::UDismembermentBoneSetup()
{
}

UDismembermentBoneSetup::~UDismembermentBoneSetup()
{
}

TArray<FName> UDismembermentBoneSetup::GetSuggestedBoneNames()
{
	TArray<FName> BoneNames;
	TArray<FName> SuggestedBones;
	DEBUG("Hello")
	for (FName Bone : BoneNames) //Check ALL Bones Against Substrings, Add them if a substring matches
	{
		bool bIsValid = false;

		//Has Valid Substr
		for (FString ValidName : ValidNameSubstrings)
			if(Bone.ToString().ToLower().Contains(ValidName) )
				bIsValid = true;

		//Has Invalid Substr
		for (FString InvalidSubstring : InvalidNameSubstrings)
			if(Bone.ToString().ToLower().Contains(InvalidSubstring))
				bIsValid = false;

		if(bIsValid)
			SuggestedBones.Add(Bone);
					
		}
	
	return SuggestedBones;
}
