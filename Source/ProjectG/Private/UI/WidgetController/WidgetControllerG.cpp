// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/WidgetControllerG.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "AbilitySystem/AttributeSetG.h"
#include "Player/PlayerControllerG.h"
#include "Player/PlayerStateG.h"

void UWidgetControllerG::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UWidgetControllerG::BroadcastInitialValues()
{
}

void UWidgetControllerG::BindCallbacksToDependencies()
{
}

APlayerControllerG* UWidgetControllerG::GetPlayerController()
{
	if (PlayerControllerG == nullptr)
	{
		PlayerControllerG = Cast<APlayerControllerG>(PlayerController);
	}
	return PlayerControllerG;
}

APlayerStateG* UWidgetControllerG::GetPlayerState()
{
	if (PlayerStateG == nullptr)
	{
		PlayerStateG = Cast<APlayerStateG>(PlayerState);
	}
	return PlayerStateG;
}

UAbilitySystemComponentG* UWidgetControllerG::GetAbilitySystemComponent()
{
	if (AbilitySystemComponentG == nullptr)
	{
		AbilitySystemComponentG = Cast<UAbilitySystemComponentG>(AbilitySystemComponent);
	}
	return AbilitySystemComponentG;
}

UAttributeSetG* UWidgetControllerG::GetAttributeSet()
{
	if (AttributeSetG == nullptr)
	{
		AttributeSetG = Cast<UAttributeSetG>(AttributeSet);
	}
	return AttributeSetG;
}
