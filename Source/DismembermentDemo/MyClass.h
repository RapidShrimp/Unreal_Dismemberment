// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyClass.generated.h"

UCLASS()
class DISMEMBERMENTDEMO_API AMyClass : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyClass();

protected:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
	TArray<FString> ValidNameSubstrings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dismemberment")
	TArray<FString> InvalidNameSubstrings;

		
	UFUNCTION(BlueprintCallable, Category = "Dismemberment")
	void FillSuggestedBoneNames();
	*/
};
