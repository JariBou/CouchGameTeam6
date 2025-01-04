// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSelectionSettings.h"

TObjectPtr<UWorld> ULevelSelectionSettings::LoadWorldAsset(const TSoftObjectPtr<UWorld>& WorldToLoad) const
{
	return WorldToLoad.LoadSynchronous();
}
