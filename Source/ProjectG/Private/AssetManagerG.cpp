// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetManagerG.h"
#include "GameplayTagG.h"

UAssetManagerG& UAssetManagerG::Get()
{
	check(GEngine);
	UAssetManagerG* AssetManagerG = Cast<UAssetManagerG>(GEngine->AssetManager);
	return *AssetManagerG;
}

void UAssetManagerG::StartInitialLoading()
{
	Super::StartInitialLoading();
	FGameplayTagG::InitNativeGameplayTags();
}