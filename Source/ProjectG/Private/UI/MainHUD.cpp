// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"

#include "UI/Widget/UserWidgetG.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void AMainHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetClass);
	check(OverlayWidgetControllerClass);
	
	UUserWidgetG* Widget = CreateWidget<UUserWidgetG>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Widget;
	
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* Controller = GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->AssignWidgetController(Controller);
	Controller->BroadcastInitialValues();

	Widget->AddToViewport();	
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

void AMainHUD::BeginPlay()
{
	// PC,PS,ASC,AS 를 초기화 하려고 PlayerController에서 정보를 가져와서 했는디..

}
