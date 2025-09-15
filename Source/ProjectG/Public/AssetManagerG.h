// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AssetManagerG.generated.h"

/**
 * DefaultEngine.ini 수정 / 에디터 ProjectSetting > AssetManagerClass 설정
 * AssetManagerClassName=/Script/ProjectG.AssetManagerG 
 */
UCLASS()
class PROJECTG_API UAssetManagerG : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAssetManagerG& Get();

	virtual void StartInitialLoading() override;
};
