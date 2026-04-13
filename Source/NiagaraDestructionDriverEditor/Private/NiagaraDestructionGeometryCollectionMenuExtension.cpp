// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetToolsModule.h"
#include "NiagaraDestructionDriverGeometryCollectionFunctions.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "ClassViewerFilter.h"
#include "NiagaraDestructionDriverEditorSettings.h"
#include "Kismet2/SClassPickerDialog.h"

#define LOCTEXT_NAMESPACE "FNiagaraDestructionDriverEditorModule"

namespace NiagaraDestructionDriver::MenuExtension_GeometryCollection
{
    class FCreateNiagaraDestructibleAssetsClassFilter : public IClassViewerFilter
	{
	public:
		FCreateNiagaraDestructibleAssetsClassFilter(UClass* InBaseClass)
			: BaseClass(InBaseClass)
			, RequiredClassFlags(CLASS_Abstract)
			, DisallowedClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown)
		{
		}
   
		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			if (InClass != nullptr)
			{
				return InClass->IsChildOf(BaseClass) && !InClass->HasAnyClassFlags(DisallowedClassFlags) && InClass->HasAnyClassFlags(RequiredClassFlags);
			}
			return false;
		}
   
		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			return InUnloadedClassData->IsChildOf(BaseClass) && !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) && InUnloadedClassData->HasAnyClassFlags(RequiredClassFlags);
		}
   
	private:
		UClass* BaseClass;
   
		EClassFlags RequiredClassFlags;
		EClassFlags DisallowedClassFlags;
	};

	static void ExecuteCreateNiagaraDestructionAssets(const FToolMenuContext& InContext)
	{
		if (const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext))
		{
			const FText TitleText = LOCTEXT("CreateNiagaraDestructionAssets_PickActorClass", "Pick Niagara Destruction Driver Actor Class");
	
			FClassViewerInitializationOptions Options;
			Options.ClassFilters.Add(MakeShared<FCreateNiagaraDestructibleAssetsClassFilter>(ANiagaraDestructionDriverActor::StaticClass()));
			Options.InitiallySelectedClass = GetDefault<UNiagaraDestructionDriverEditorSettings>()->DefaultDestructionDriverActorClass.LoadSynchronous();
	
			UClass* PickedClass = nullptr;
			const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, OUT PickedClass, ANiagaraDestructionDriverActor::StaticClass());
			
			if (bPressedOk && PickedClass != nullptr)
			{
				TArray<UGeometryCollection*> GeometryCollections = Context->LoadSelectedObjects<UGeometryCollection>();
				if (!GeometryCollections.IsEmpty())
				{
					for (UGeometryCollection* GeometryCollection : GeometryCollections)
					{
						if (GeometryCollection)
						{
							UNiagaraDestructionDriverGeometryCollectionFunctions::GeometryCollectionToNiagaraDestructible(GeometryCollection, PickedClass);
						}
					}
				}
			}
		}
	}

	static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, [] {
		UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
				{
					FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
					UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UGeometryCollection::StaticClass());
					FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
					{
						const TAttribute<FText> Label = LOCTEXT("CreateNiagaraDestructionAssets_Label", "Create Niagara Destruction Driver Assets");
						const TAttribute<FText> ToolTip = LOCTEXT("CreateNiagaraDestructionAssets_Tooltip", "Creates a data asset pointing to a new static mesh and initial bone locations texture for the selected geometry collection.");
						const TAttribute<FSlateIcon> Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.ParticleSystemComponent");

						FToolUIAction UIAction;
						UIAction.ExecuteAction = FToolMenuExecuteAction::CreateStatic(&ExecuteCreateNiagaraDestructionAssets);
						Section.AddMenuEntry("GC_CreateNiagaraDestructionAssets", Label, ToolTip, Icon, UIAction);
					}
				}));
		});
}
#undef LOCTEXT_NAMESPACE
