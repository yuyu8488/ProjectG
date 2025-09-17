// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"


#include "Player/PlayerControllerG.h"
#include "Player/PlayerStateG.h"
#include "UI/Widget/UserWidgetG.h"
#include "UI/WidgetController/OverlayWidgetController.h"

AMainHUD::AMainHUD()
{
	
}

UOverlayWidgetController* AMainHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

void AMainHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetControllerClass);
	
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* Controller = GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->AssignWidgetController(Controller);
	Controller->BroadcastInitialValues();
}

void AMainHUD::BeginPlay()
{
	// OverlayWidget생성.
	check(OverlayWidgetClass);
	OverlayWidget = CreateWidget<UUserWidgetG>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();	
}



