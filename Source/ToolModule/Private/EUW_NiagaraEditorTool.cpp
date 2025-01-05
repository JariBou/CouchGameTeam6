// Fill out your copyright notice in the Description page of Project Settings.


#include "EUW_NiagaraEditorTool.h"
#include "EUW_NiagaraEditorTool.h"
#include "Factories/MaterialParameterCollectionFactoryNew.h"
#include "AssetToolsModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorUtilityLibrary.h"
#include "IContentBrowserSingleton.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/BlueprintFactory.h"

void UEUW_NiagaraEditorTool::GetNiagaraInfo()
{
	SelectedItems = UEditorUtilityLibrary::GetSelectedAssets();
	FString Path = "/Game/_project/Tools/NiagaraCollectionExport/Test";
	bool bSuccess = false;
	FString OutMsg;
	UserParams = CreateAssetNiagaraParameterCollection(Path, bSuccess, OutMsg);
	
	if(SelectedItems.Num() == 1)
	{
		
		SelectedNiagaraSystem = Cast<UNiagaraSystem>(SelectedItems[0]);
		
		if(SelectedNiagaraSystem)
		{
			UE_LOG(LogTemp, Warning, TEXT("NiagaraWorking"));
			
			/*
			FNiagaraVariable hey = NiagaraVariables[0];
			hey.GetType().GetClass();
			*/
			SelectedNiagaraSystem->GetExposedParameters().GetParameters(NiagaraParameterTest);
			
			/*
			FString NameNiagara = UserParams[0].GetType().GetClass()->GetAuthoredName();
			FText::AsCultureInvariant(NameNiagara);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *NameNiagara);
			*/
			
			for (FNiagaraVariable SelectedNiagaraVar : NiagaraParameterTest)
			{
				/*
				const int32* offset = SelectedNiagaraSystem->GetExposedParameters().FindParameterOffset(SelectedNiagaraVar);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::FromInt(*offset));
				DataInterFace.Add(SelectedNiagaraSystem->GetExposedParameters().GetDataInterface(*offset));
				*/
				UserParams->AddParameter(SelectedNiagaraVar);
			}
			/*
			FName name = UserParams->GetParameters()[0].GetName();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *name.ToString());
			*/
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not a Niagara"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Select One Niagara"));
	}

}

UObject* UEUW_NiagaraEditorTool::CreateAsset(FString AssetPath, UClass* AssetClass,
	UFactory* AssetFactory, bool& bOutSuccess, FString& OutInfoMessage)
{
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	UFactory* Factory = AssetFactory;
	if(Factory == nullptr)
	{
		for (UFactory* Fac : AssetTools.GetNewAssetFactories())
		{
			if(Fac->SupportedClass == AssetClass)
			{
				Factory = Fac;
				break;
			}
		}

		if(Factory == nullptr)
		{
			bOutSuccess = false;
			OutInfoMessage = FString::Printf(TEXT("Create Failed : '%s'"), *AssetPath);
			UE_LOG(LogTemp, Warning, TEXT("Create Failed : '%s'"), *AssetPath);

			return nullptr;
		}
	}

	if(Factory->SupportedClass != AssetClass)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("CreateFailed case 2 : '%s'"), *AssetPath);
		UE_LOG(LogTemp, Warning, TEXT("Create Failed case 2 : '%s'"), *AssetPath);

		return nullptr;
	}

	UObject* Asset = AssetTools.CreateAsset(FPaths::GetBaseFilename(AssetPath), FPaths::GetPath(AssetPath), AssetClass, Factory);

	if (Asset == nullptr)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Failed case 3 : '%s'"), *AssetPath);
		UE_LOG(LogTemp, Warning, TEXT("Create Failed case 3 : '%s'"), *AssetPath);

		return nullptr;
	}

	bOutSuccess = true;
	return Asset;
}

UNiagaraParameterCollection* UEUW_NiagaraEditorTool::CreateAssetNiagaraParameterCollection(FString AssetPath,
	bool& bOutSuccess, FString& OutInfoMessage)
{
	UObject* Asset = CreateAsset(AssetPath, UNiagaraParameterCollection::StaticClass(), nullptr, bOutSuccess, OutInfoMessage);
	return Cast<UNiagaraParameterCollection>(Asset);
}

