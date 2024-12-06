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

	for (int i = 0; i < 4; ++i)
	{
		const TSharedRef<SComboButton> CharacterSelectComboButton = SNew(SComboButton)
			.OnGetMenuContent_Lambda([this, i]
			{
				FMenuBuilder MenuBuilder = FMenuBuilder(true, nullptr);

				TArray<FName> PossibleInputs = {
					FName(TEXT("T1 Knight")),
					FName(TEXT("T1 Squire")),
					FName(TEXT("T2 Knight")),
					FName(TEXT("T2 Squire")),
				};
				
				UCharacterSettings* CharactersSettings = GetMutableDefault<UCharacterSettings>();
				CharactersSettings->DefaultSpawnInfo.Add(i, EDefaultSpawnInfo::NoDefaultSpawnInfo);

				FName EmptyCharacterID;
				MenuBuilder.AddMenuEntry(
					FText::FromName(EmptyCharacterID),
					FText::FromName(EmptyCharacterID),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([this, i, EmptyCharacterID]
					{
						UCharacterSettings* CharacterSettings = GetMutableDefault<UCharacterSettings>();
						CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::NoDefaultSpawnInfo;
					})));

				for (FName& CharacterID : PossibleInputs)
				{
					MenuBuilder.AddMenuEntry(
						FText::FromName(CharacterID),
						FText::FromName(CharacterID),
						FSlateIcon(),
						FUIAction(FExecuteAction::CreateLambda([this, i, CharacterID]
						{
							UCharacterSettings* CharacterSettings = GetMutableDefault<UCharacterSettings>();

							if (CharacterID == FName(TEXT("T1 Knight")))
							{
								CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team1_K;
							} else if (CharacterID == FName(TEXT("T1 Squire")))
							{
								CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team1_S;
							} else if (CharacterID == FName(TEXT("T2 Knight")))
							{
								CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team2_K;
							} else if (CharacterID == FName(TEXT("T2 Squire")))
							{
								CharacterSettings->DefaultSpawnInfo[i] = EDefaultSpawnInfo::Team2_S;
							}

							UE_LOG(LogTemp, Warning, TEXT("Smash Character ID Selected = %s"), *CharacterID.ToString());
						}))
					);
				}
				return MenuBuilder.MakeWidget();
			})
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text_Lambda([this, i]
				{
					const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
					if (!CharacterSettings->DefaultSpawnInfo.Contains(i)) return FText::FromString("None");

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