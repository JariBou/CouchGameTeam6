// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "NiagaraParameterCollection.h"
#include "EUW_NiagaraEditorTool.generated.h"

class UNiagaraParameterCollection;
/**
 * 
 */
UCLASS()
class TOOLMODULE_API UEUW_NiagaraEditorTool : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, BlueprintCallable)
	void GetNiagaraInfo();

	UPROPERTY()
	TArray<UObject*> SelectedItems;

	UPROPERTY()
	UNiagaraSystem* SelectedNiagaraSystem;
	
	UPROPERTY(BlueprintReadWrite, Category="NiagaraTool")
	UNiagaraParameterCollection* UserParams;

	
	UPROPERTY(BlueprintReadWrite, Category="NiagaraTool")
	TArray<FNiagaraVariable> NiagaraParameterTest;

	/*
	UPROPERTY(BlueprintReadWrite, Category="NiagaraTool")
	TArray<UNiagaraDataInterface*> DataInterFace;
	*/

	UFUNCTION(BlueprintCallable)
	static UObject* CreateAsset(FString AssetPath, UClass* AssetClass, UFactory* AssetFactory, bool& bOutSuccess, FString& OutInfoMessage);

	UFUNCTION(BlueprintCallable)
	static UNiagaraParameterCollection* CreateAssetNiagaraParameterCollection(FString AssetPath, bool& bOutSuccess, FString& OutInfoMessage);

};
