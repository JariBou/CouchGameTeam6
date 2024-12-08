#include "ToolModule.h"

#include <ToolMenus.h>

#include "Characters/CharacterSettings.h"

#define LOCTEXT_NAMESPACE "FToolModuleModule"

void FToolModuleModule::StartupModule()
{
	// Register a function to be called when menu system is initialized
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FToolModuleModule::RegisterMenuExtensions));
}

void FToolModuleModule::ShutdownModule()
{
	// Unregister the startup function
	UToolMenus::UnRegisterStartupCallback(this);
 
	// Unregister all our menu extensions
	UToolMenus::UnregisterOwner(this);
}

void FToolModuleModule::RegisterMenuExtensions()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Play");

	TArray<FName> PossibleInputs = {
		FName(TEXT("None")),
		FName(TEXT("T1 Knight")),
		FName(TEXT("T1 Squire")),
		FName(TEXT("T2 Knight")),
		FName(TEXT("T2 Squire")),
	};

	for (int i = 0; i < 4; ++i)
	{
		UCharacterSettings* CharacterSettings = GetMutableDefault<UCharacterSettings>();
		if (!CharacterSettings->DefaultSpawnInfo.Contains(i)) CharacterSettings->DefaultSpawnInfo.Add(i, static_cast<TEnumAsByte<EDefaultSpawnInfo>>(i+1));
		
		const TSharedRef<SComboButton> CharacterSelectComboButton = SNew(SComboButton)
			.OnGetMenuContent_Lambda([this, i, PossibleInputs]
			{
				FMenuBuilder MenuBuilder = FMenuBuilder(true, nullptr);
				
				// FName EmptyCharacterID;
				// MenuBuilder.AddMenuEntry(
				// 	FText::FromName(EmptyCharacterID),
				// 	FText::FromName(EmptyCharacterID),
				// 	FSlateIcon(),
				// 	FUIAction(FExecuteAction::CreateLambda([this, i, EmptyCharacterID]
				// 	{
				// 		UCharacterSettings* CharacterSettings = GetMutableDefault<UCharacterSettings>();
				// 		CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::NoDefaultSpawnInfo;
				// 	})));

				for (const FName& CharacterID : PossibleInputs)
				{
					MenuBuilder.AddMenuEntry(
						FText::FromName(CharacterID),
						FText::FromName(CharacterID),
						FSlateIcon(),
						FUIAction(FExecuteAction::CreateLambda([this, i, CharacterID, PossibleInputs]
						{
							UCharacterSettings* CharacterSettings = GetMutableDefault<UCharacterSettings>();

							CharacterSettings->DefaultSpawnInfo[i] = static_cast<TEnumAsByte<EDefaultSpawnInfo>>(PossibleInputs.IndexOfByKey(CharacterID));

							// if (CharacterID == FName(TEXT("T1 Knight")))
							// {
							// 	CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team1_K;
							// } else if (CharacterID == FName(TEXT("T1 Squire")))
							// {
							// 	CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team1_S;
							// } else if (CharacterID == FName(TEXT("T2 Knight")))
							// {
							// 	CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team2_K;
							// } else if (CharacterID == FName(TEXT("T2 Squire")))
							// {
							// 	CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team2_S;
							// }

							UE_LOG(LogTemp, Warning, TEXT("Smash Character ID Selected = %s"), *CharacterID.ToString());
						}))
					);
				}
				return MenuBuilder.MakeWidget();
			})
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text_Lambda([this, i, PossibleInputs]
				{
					const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
					if (!CharacterSettings->DefaultSpawnInfo.Contains(i)) return FText::FromString("None");

					return FText::FromName(PossibleInputs[static_cast<int>(CharacterSettings->DefaultSpawnInfo[i])]);

					switch(CharacterSettings->DefaultSpawnInfo[i])
					{
						case Team1_K:
							return FText::FromString("T1 Knight");
						case Team1_S:
							return FText::FromString("T1 Squire");
						case Team2_K:
							return FText::FromString("T2 Knight");
						case Team2_S:
							return FText::FromString("T2 Squire");
						case NoDefaultSpawnInfo:
						default:
							return FText::FromString("None");
					}
				})
			];

		FString ComboString = FString::Printf(TEXT("SmashCharacterCombo_%d"), i);
		FName ComboName = FName(*ComboString);
		ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(ComboName, CharacterSelectComboButton, FText::FromString(ComboString)));
	}

	const TSharedRef<STextBlock> CheckBoxLabel = SNew(STextBlock)
		.SimpleTextMode(false)
		.Margin(FMargin(3, 10, 0, 10))
		.Text(INVTEXT("Activate:"));
	FString CheckBoxLabelString = FString::Printf(TEXT("CheckBoxLabelString"));
	FName CheckBoxLabelName = FName(*CheckBoxLabelString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxLabelName, CheckBoxLabel, FText::FromString(CheckBoxLabelString)));
	
	const TSharedRef<SCheckBox> Checkbox = SNew(SCheckBox)
		.OnCheckStateChanged_Lambda([&](ECheckBoxState CheckState)
		{
			UCharacterSettings* CharactersSettings = GetMutableDefault<UCharacterSettings>();

			CharactersSettings->UseDefaultSpawnInfo = CheckState == ECheckBoxState::Checked;
		});
	FString CheckboxString = FString::Printf(TEXT("CheckBoxString"));
	FName CheckboxName = FName(*CheckboxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckboxName, Checkbox, FText::FromString(CheckboxString)));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FToolModuleModule, ToolModule)