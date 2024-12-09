// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterSettings.h"
#include "Characters/SfCharacterInputData.h"
#include "Characters/Controllers/MenuInputData.h"

TObjectPtr<USfCharacterInputData> UCharacterSettings::GetInputDataFromPlayerType(TEnumAsByte<TypeOfPlayer> Type) const
{
	return CharacterInputDatas.Find(Type)->InputData.LoadSynchronous();
}

TObjectPtr<UMenuInputData> UCharacterSettings::GetMenuInputData() const
{
	return MenuInputData.LoadSynchronous();
}
