#include "LevelSelectionTool.h"

#include "LevelSelectionSettings.h"

#define LOCTEXT_NAMESPACE "FLevelSelectionToolModule"

void FLevelSelectionToolModule::StartupModule()
{
	// Register a function to be called when menu system is initialized
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(
		this, &FLevelSelectionToolModule::RegisterMenuExtensions));
}

void FLevelSelectionToolModule::ShutdownModule()
{
	// Unregister the startup function
	UToolMenus::UnRegisterStartupCallback(this);
 
	// Unregister all our menu extensions
	UToolMenus::UnregisterOwner(this);
}

void FLevelSelectionToolModule::RegisterMenuExtensions()
{
	//Getting menu and toolbar where we want to add button
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Play");

	//Array to keep all listed levels names
	TArray<FName> PossibleLevels = {
		FName(TEXT("None"))
	};
	
	ULevelSelectionSettings* LevelSelectionSettings = GetMutableDefault<ULevelSelectionSettings>();

	//Initiate it with None so it isn't null just in case
	LevelSelectionSettings->SelectedLevelName = PossibleLevels[0];

	//Adding Levels name to list
	for(const TSoftObjectPtr<UWorld> Level : LevelSelectionSettings->Levels)
	{
		PossibleLevels.Add(FName(*Level.GetAssetName()));
	}

	//Creating button
	const TSharedRef<SComboButton> LevelSelectionButton = SNew(SComboButton)
		.OnGetMenuContent_Lambda([this, PossibleLevels]
		{
			FMenuBuilder MenuBuilder = FMenuBuilder(true, nullptr);

			//Add a menu entry for each level name listed
			for(const FName& Level : PossibleLevels)
			{
				MenuBuilder.AddMenuEntry(
					FText::FromName(Level),
					FText::FromName(Level),
					FSlateIcon(),
					//Changing selected level when interacting with a button of the list
					FUIAction(FExecuteAction::CreateLambda([this, Level, PossibleLevels]
					{
						ULevelSelectionSettings* LevelSelectionSettings = GetMutableDefault<ULevelSelectionSettings>();

						LevelSelectionSettings->SelectedLevelName = Level;
						
						UE_LOG(LogTemp, Warning, TEXT("LevelSelected = %s"), *Level.ToString());
					}))
				);
			}
			//Creating widget
			return MenuBuilder.MakeWidget();
		})
		//Setting first button content to know which level is selected
		.ButtonContent()
		[
			SNew(STextBlock).Text_Lambda([this, PossibleLevels]
			{
				ULevelSelectionSettings* LevelSelectionSettings = GetMutableDefault<ULevelSelectionSettings>();
				
				return FText::FromName(LevelSelectionSettings->SelectedLevelName);
			})
		];

	//Adding button to desired toolbar
	FString ButtonString = FString::Printf(TEXT("LevelSelection"));
	FName ButtonName = FName(*ButtonString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(ButtonName, LevelSelectionButton, FText::FromString(ButtonString)));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLevelSelectionToolModule, LevelSelectionTool)