// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NiagaraDestructionDriverActor.h"
#include "Engine/DeveloperSettings.h"
#include "NiagaraDestructionDriverEditorSettings.generated.h"


UCLASS(Config=Editor, DefaultConfig)
class NIAGARADESTRUCTIONDRIVEREDITOR_API UNiagaraDestructionDriverEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category="Asset Action", meta=(AllowAbstract))
	TSoftClassPtr<ANiagaraDestructionDriverActor> DefaultDestructionDriverActorClass = ANiagaraDestructionDriverActor::StaticClass();
};
