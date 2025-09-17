// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UUserWidgetG;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class PROJECTG_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& Params);

	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UUserWidgetG> OverlayWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidgetG> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

};
