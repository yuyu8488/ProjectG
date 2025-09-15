// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTG_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsInCombat();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetInCombat(const bool InCombat);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateTargets();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdatePrimaryTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetPrimaryTarget();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FacePrimaryTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetProjectileSocketTransform(FName InSocketName);

	
};