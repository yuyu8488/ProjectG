// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetG.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTG_API UUserWidgetG : public UUserWidget
{
	GENERATED_BODY()

public:
	// SetWidgetController
	UFUNCTION(BlueprintCallable)
	void AssignWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	// WidgetControllerSet
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetControllerSet();
};
