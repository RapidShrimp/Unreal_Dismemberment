// Fill out your copyright notice in the Description page of Project Settings.


#include "DismembermentBoneSetup.h"
#define DEBUG(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

UDismembermentBoneSetup::UDismembermentBoneSetup()
{
}

UDismembermentBoneSetup::~UDismembermentBoneSetup()
{
}

TArray<FString> UDismembermentBoneSetup::ReadSubstringFromFile(FString Filepath)
{
	TArray<FString> FoundNameStrings;
	if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*Filepath))
	{
		DEBUG("Read String From File Failed - File Doesnt Exist");
		return FoundNameStrings;
	}

	if(!FFileHelper::LoadFileToStringArray(FoundNameStrings,*Filepath))
	{
		DEBUG("Read String From File Failed - Unable to Read filetype (Non TXT file)");
		return FoundNameStrings;
	}
	return FoundNameStrings;
}

bool UDismembermentBoneSetup::DoesBoneExist(FName Bone)
{
	TArray<FName> BoneNames;
	BoneNames.Empty(Mesh->GetRefSkeleton().GetNum());
	BoneNames.AddUninitialized(Mesh->GetRefSkeleton().GetNum());
	for (int32 i = 0; i < Mesh->GetRefSkeleton().GetNum(); i++)
	{
		if(Mesh->GetRefSkeleton().GetBoneName(i) == Bone)
			return true;
	}
	return false;
}

TArray<FName> UDismembermentBoneSetup::GetSuggestedBoneNames()
{
	
	ValidNameSubstrings = ReadSubstringFromFile(ValidSubstrFilepath);
	InvalidNameSubstrings = ReadSubstringFromFile(InvalidSubstrFilepath);


	//TODO Get Bone Names
	
	TArray<FName> BoneNames;
	// pre-size the array to avoid unnecessary reallocation
	BoneNames.Empty(Mesh->GetRefSkeleton().GetNum());
	BoneNames.AddUninitialized(Mesh->GetRefSkeleton().GetNum());
	for (int32 i = 0; i < Mesh->GetRefSkeleton().GetNum(); i++)
	{
		BoneNames[i] = Mesh->GetRefSkeleton().GetBoneName(i);
	}

	TArray<FName> SuggestedBones;
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
