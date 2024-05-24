// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "DismembermentBoneSetup.generated.h"

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

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Dismembmerment")
	TObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(BlueprintReadOnly, Category = "Dismemberment")
	FString ValidSubstrFilepath = "E:/University/Personal Projects/Stealth/Unreal_Dismemberment/Plugins/SkeleDismemberEditor/Source/SkeleDismemberEditor/Substrings/ValidBoneSubstrings.txt";

	UPROPERTY(BlueprintReadOnly, Category = "Dismemberment")
	FString InvalidSubstrFilepath = "E:/University/Personal Projects/Stealth/Unreal_Dismemberment/Plugins/SkeleDismemberEditor/Source/SkeleDismemberEditor/Substrings/InValidBoneSubstrings.txt";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> ValidNameSubstrings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> InvalidNameSubstrings;

	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dismemberment")
	TArray<FString> ReadSubstringFromFile(FString Filepath);

	UFUNCTION(BlueprintCallable,CallInEditor,Category = "Dismemberment")
	bool DoesBoneExist(FName Bone);
    UFUNCTION(BlueprintCallable,CallInEditor, Category = "Dismemberment")
    TArray<FName> GetSuggestedBoneNames();

};
