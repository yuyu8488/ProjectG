// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/UserWidgetG.h"


void UUserWidgetG::AssignWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	OnWidgetControllerSet();
}
