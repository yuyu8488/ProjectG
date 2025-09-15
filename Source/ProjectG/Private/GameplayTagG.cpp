	// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTagG.h"
#include "GameplayTagsManager.h"

FGameplayTagG FGameplayTagG::GameplayTags;

void FGameplayTagG::InitNativeGameplayTags()
{
	/** Vital */
	GameplayTags.Attribute_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.Health"));
	GameplayTags.Attribute_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.MaxHealth"));
	GameplayTags.Attribute_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.Mana"));
	GameplayTags.Attribute_Vital_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.MaxMana"));

	
	GameplayTags.Data_GameplayAbility_DamagePercent = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Data.GameplayAbility.DamagePercent"));

	
	GameplayTags.Character_State_Idle = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.State.Idle"));
	GameplayTags.Character_State_Walking = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.State.Walking"));
	GameplayTags.Character_State_Attacking = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.State.Attacking"));


	// Ability Combo
	GameplayTags.Ability_Attack_Once= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack.Once"));
	GameplayTags.Ability_Attack_Combo= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack.Combo"));
	GameplayTags.Ability_BasicAttack= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack"));
	GameplayTags.Ability_BasicAttack_ComboWindow= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack.ComboWindow"));
	GameplayTags.Ability_BasicAttack_Combo0 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack.Combo0"));
	GameplayTags.Ability_BasicAttack_Combo1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack.Combo1"));
	GameplayTags.Ability_BasicAttack_Combo2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack.Combo2"));
	GameplayTags.Ability_BasicAttack_Combo3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.BasicAttack.Combo3"));
}
