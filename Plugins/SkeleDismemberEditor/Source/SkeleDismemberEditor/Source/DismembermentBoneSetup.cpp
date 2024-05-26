// Fill out your copyright notice in the Description page of Project Settings.


#include "DismembermentBoneSetup.h"

#include "Components/DetailsView.h"
#include "Components/VerticalBox.h"

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

	TArray<FName> BoneNames;
	// pre-size the array to avoid unnecessary reallocation
	BoneNames.Empty(Mesh->GetRefSkeleton().GetNum());
	BoneNames.AddUninitialized(Mesh->GetRefSkeleton().GetNum());
	for (int32 i = 0; i < Mesh->GetRefSkeleton().GetNum(); i++)
	{
		BoneNames[i] = Mesh->GetRefSkeleton().GetBoneName(i);
	}

	SuggestedBones.Empty();
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

int UDismembermentBoneSetup::GetLimbIndex(FName BoneName)
{
	for(int i = 0; i<Limbs.Num();i++)
	{
		if(Limbs[i].LimbRootName == BoneName)
		{
			return i;
		}
	}
	return -1;
}

void UDismembermentBoneSetup::OnSkeletonChanged(FName PropertyValue)
{
	if(PropertyValue != "Mesh")
		return;
	
	InitialiseNewSkeleton();
}

void UDismembermentBoneSetup::ClearData_Implementation()
{
	ActiveBones.Empty();
	SuggestedBones.Empty();
	Limbs.Empty();
	VB_SuggestedBoneNames->ClearChildren();
	VB_SelectedBones->ClearChildren();
	DV_LimbData->SetVisibility(ESlateVisibility::Collapsed);
}

void UDismembermentBoneSetup::InitialiseNewSkeleton()
{
	ClearData();
	GetSuggestedBoneNames();
	DV_LimbData->SetObject(this);
	for (int i = 0; i<SuggestedBones.Num();i++)
	{
		OnCreateWidget.Execute(true,SuggestedBones[i],VB_SuggestedBoneNames);
	}
}

void UDismembermentBoneSetup::AddLimb(FName BoneName, UUserWidget* OptionWidget)
{
	if(OptionWidget == nullptr)
	{
		if(!DoesBoneExist(BoneName))
		{
			OnIncorrectInput.Execute();
			return;
		}
	}

	if(ActiveBones.Contains(BoneName))
		return;

	ActiveBones.Add(BoneName);
	FLimbGroupData NewLimb;
	NewLimb.LimbRootName = BoneName;
	Limbs.Add(NewLimb);
	OnCreateWidget.Execute(false,BoneName,VB_SelectedBones);
}

void UDismembermentBoneSetup::RemoveLimb(FName BoneName, UUserWidget* OptionWidget)
{
	ActiveBones.Remove(BoneName);
	OptionWidget->RemoveFromParent();

	//If index Exists Remove it
	int Index = GetLimbIndex(BoneName);
	if(Index == -1)
		return;
	Limbs.RemoveAt(Index);
	CurrentIndex = -1;
}

void UDismembermentBoneSetup::UpdateLimbPanel(FName SelectedBone)
{
	if(SelectedBone == "None")
	{
		DV_LimbData->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	int Index = GetLimbIndex(SelectedBone);
	if(Index==-1)
		return;

	CurrentIndex = Index;
	OnUpdatePanel.Execute(Limbs[Index]); 
	DV_LimbData->SetVisibility(ESlateVisibility::Visible);
	DV_LimbData->SetObject(this);
}

void UDismembermentBoneSetup::ChangeLimbData(int Index, FLimbGroupData NewData)
{
	if(GetLimbIndex(NewData.LimbRootName) == Index)
	{
		Limbs[Index] = NewData;
	}
}

void UDismembermentBoneSetup::CreateAsset()
{
	UE_LOG(LogTemp,Warning,TEXT("Create Asset Called"));
}
