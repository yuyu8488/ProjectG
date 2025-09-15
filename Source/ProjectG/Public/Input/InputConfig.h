// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem//AbilityInputID.h"
#include "Engine/DataAsset.h"
#include "InputConfig.generated.h"

USTRUCT(BlueprintType)
struct FInputActionG
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	EAbilityInputID AbilityInputID = EAbilityInputID::None;
};

UCLASS()
class PROJECTG_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FInputActionG> AbilityInputActions;
};