// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerStateG.h"

#include "AbilitySystem/AbilitySystemComponentG.h"
#include "AbilitySystem/AttributeSetG.h"
#

APlayerStateG::APlayerStateG()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentG>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAttributeSetG>("AttributeSet");

	SetNetUpdateFrequency(100.f);
}