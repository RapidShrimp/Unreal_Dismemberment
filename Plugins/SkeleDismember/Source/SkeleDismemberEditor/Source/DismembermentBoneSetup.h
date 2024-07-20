// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SkeleDismember/Structs/LimbGroupData.h"
#include "DismembermentBoneSetup.generated.h"

class UDetailsView;
class UVerticalBox;
struct FLimbGroupData;


DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnCreateWidget, bool, isAddOption, FName, BoneName, UVerticalBox*, Placement);
DECLARE_DYNAMIC_DELEGATE(FOnIncorrectInput);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatePanel, FLimbGroupData, Limb);

/**
 * 
 */
UCLASS(BlueprintType)
class SKELEDISMEMBEREDITOR_API UDismembermentBoneSetup : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UDismembermentBoneSetup();
	~UDismembermentBoneSetup();

protected:

	//Asset References
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Dismembmerment")
	TObjectPtr<USkeletalMesh> Mesh;

	//Substrings
	UPROPERTY(BlueprintReadOnly, Category = "Dismemberment")
	FString ValidSubstrFilepath = "E:/University/Personal Projects/Stealth/Unreal_Dismemberment/Plugins/SkeleDismember/Source/SkeleDismemberEditor/Substrings/ValidBoneSubstrings.txt";

	UPROPERTY(BlueprintReadOnly, Category = "Dismemberment")
	FString InvalidSubstrFilepath = "E:/University/Personal Projects/Stealth/Unreal_Dismemberment/Plugins/SkeleDismember/Source/SkeleDismemberEditor/Substrings/InValidBoneSubstrings.txt";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> ValidNameSubstrings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> InvalidNameSubstrings;

	//Widgets Required
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Dismemberment")
	TObjectPtr<UVerticalBox> VB_SuggestedBoneNames;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Dismemberment")
	TObjectPtr<UVerticalBox> VB_SelectedBones;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Dismemberment")
	TObjectPtr<UVerticalBox> VB_ActiveBone;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Dismemberment")
	TObjectPtr<UDetailsView> DV_LimbData;	

	//Delegates
	UPROPERTY(BlueprintReadWrite,Category = "Dismemberment")
	FOnCreateWidget OnCreateWidget;
	
	UPROPERTY(BlueprintReadWrite,Category = "Dismemberment")
	FOnIncorrectInput OnIncorrectInput;

	UPROPERTY(BlueprintReadWrite,Category = "Dismemberment")
	FOnUpdatePanel OnUpdatePanel;


	//Functions
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dismemberment")
	TArray<FString> ReadSubstringFromFile(FString Filepath);

	UFUNCTION(BlueprintCallable,CallInEditor,Category = "Dismemberment")
	bool DoesBoneExist(FName Bone);

    UFUNCTION(BlueprintCallable,CallInEditor, Category = "Dismemberment")
    TArray<FName> GetSuggestedBoneNames();

	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	int GetLimbIndex(FName BoneName);
	
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void OnSkeletonChanged(FName PropertyValue);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,CallInEditor,Category="Dismemberment")
	void ClearData();
	
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void InitialiseNewSkeleton();
	
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void AddLimb(FName BoneName, UUserWidget* OptionWidget);

	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void RemoveLimb(FName BoneName, UUserWidget* OptionWidget);

	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void UpdateLimbPanel(FName SelectedBone);

	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment")
	void ChangeLimbData(int Index, FLimbGroupData NewData);
	
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Dismemberment") //Todo Currently Unimplemented
	void CreateAsset();
	//Bone and Limb Variables
	UPROPERTY(BlueprintReadWrite,Category = "Dismemberment")
	TArray<FLimbGroupData> Limbs;

	UPROPERTY(BlueprintReadWrite,Category = "Dismemberment")
	int CurrentIndex;
	
	UPROPERTY(BlueprintReadOnly, Category = "Dismemberment")
	TArray<FName> ActiveBones;
	
	UPROPERTY(BlueprintReadWrite, Category = "Dismemberment")
	TArray<FName> SuggestedBones;
};
