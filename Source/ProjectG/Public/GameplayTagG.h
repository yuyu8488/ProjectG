// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/** Singleton containing native GameplayTags */
struct FGameplayTagG
{
public:
	static const FGameplayTagG& Get() {return GameplayTags;}
	static void InitNativeGameplayTags();
	
	/** Attribute Vital */
	FGameplayTag Attribute_Vital_Health;
	FGameplayTag Attribute_Vital_MaxHealth;
	FGameplayTag Attribute_Vital_Mana;
	FGameplayTag Attribute_Vital_MaxMana;

	// Data
	FGameplayTag Data_GameplayAbility_DamagePercent;

	// Character
	FGameplayTag Character_State_Idle;
	FGameplayTag Character_State_Walking;
	FGameplayTag Character_State_Attacking;
	FGameplayTag Character_State_AttackCancellable;

	// Ability
	FGameplayTag Ability_Attack_Once;
	FGameplayTag Ability_Attack_Combo;
	FGameplayTag Ability_BasicAttack;
	FGameplayTag Ability_BasicAttack_ComboWindow;
	FGameplayTag Ability_BasicAttack_Combo0;
	FGameplayTag Ability_BasicAttack_Combo1;
	FGameplayTag Ability_BasicAttack_Combo2;
	FGameplayTag Ability_BasicAttack_Combo3;

private:
	static FGameplayTagG GameplayTags;
};
