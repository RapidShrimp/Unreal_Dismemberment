// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "DismembermentBoneSetup.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SKELEDISMEMBER_API UDismembermentBoneSetup : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UDismembermentBoneSetup();
	~UDismembermentBoneSetup();

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Dismembmerment")
	TObjectPtr<USkeletalMesh> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> ValidNameSubstrings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
    TArray<FString> InvalidNameSubstrings;

    	
    UFUNCTION(BlueprintCallable,CallInEditor, Category = "Dismemberment")
    TArray<FName> GetSuggestedBoneNames();
    
};
