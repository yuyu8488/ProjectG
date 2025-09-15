// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbilityG.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagG.h"
#include "AbilitySystem/AbilityTypes.h"
#include "Interface/CombatInterface.h"

UGameplayAbilityG::UGameplayAbilityG()
{
	// 어빌리티가 캐릭터마다 별도의 인스턴스를 가지게 설정 >> 멤버변수가 다른 캐릭터에 영향 주지x 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FDamageEffectParams UGameplayAbilityG::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, const FGameplayTag InAbilityTag) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.GameplayEffectClass = Info.DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.DamageType = Info.DamageType;
	Params.BaseDamage = 0.f;
	Params.DamageMultiplier = Info.DamageMultiplier.GetValueAtLevel(GetAbilityLevel());
	
	return Params;
}

void UGameplayAbilityG::ApplyDamageEffect(AActor* InTargetActor, const FGameplayTag InAbilityTag)
{
	FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(InTargetActor, InAbilityTag);

	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle =
		DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
			DamageEffectParams.GameplayEffectClass,
			EffectContextHandle.GetAbilityLevel(),
			EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTagG::Get().Data_GameplayAbility_DamagePercent, DamageEffectParams.DamageMultiplier);

	// Source와 Target Character의 bInCombat = true 설정.
	if (ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatarActor))
	{
		SourceCombatInterface->Execute_SetInCombat(SourceAvatarActor, true);
	}
	if (ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(DamageEffectParams.TargetAbilitySystemComponent->GetAvatarActor()))
	{
		TargetCombatInterface->Execute_SetInCombat(DamageEffectParams.TargetAbilitySystemComponent->GetAvatarActor(), true);
	}
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGameplayAbilityG::OnComboSave()
{
}

void UGameplayAbilityG::OnComboEnd()
{
}

void UGameplayAbilityG::ResetCombo()
{
}
