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
 *	(멀티)순서대로 호출됨
 *  1) PlayerCharacter::PossessedBy()
 *  2) HUD::BeginPlay()
 *  3) PlayerController::OnRep_PlayerState()
 *  4) PlayerCharacter::OnRep_PlayerState() 
 */

UCLASS()
class PROJECTG_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainHUD();
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& Params);
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

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
