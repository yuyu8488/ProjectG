// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"			
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagG.h"
#include "AbilitySystem/AbilityTypes.h"
#include "Interface/CombatInterface.h"

FGameplayEffectContextHandle UAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	if (!DamageEffectParams.GameplayEffectClass)
	{
		return FGameplayEffectContextHandle();
	}
	
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	if (!SourceAvatarActor->HasAuthority())
	{
		return FGameplayEffectContextHandle();
	}

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	const FGameplayEffectSpecHandle SpecHandle =
		DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
			DamageEffectParams.GameplayEffectClass,
			EffectContextHandle.GetAbilityLevel(),
			EffectContextHandle);
	
	/*  비유를 통한 정리
  자동차를 만드는 과정에 비유해 보겠습니다.

   1. `AssignTagSetByCallerMagnitude`: 자동차 공장(Source ASC)에서 차체(Spec)를 만들고, "이 차는 휘발유(Tag)를 사용하고, 기본 엔진 출력은
	  200마력(Magnitude)이다" 라고 명패를 붙여주는 과정입니다. 아직 엔진이 완성된 것은 아닙니다.

   2. `UExecCalc_Damage`: 이 차체와 명패를 받은 최종 조립 라인(Execution Calculation)입니다. 여기서 "아, 휘발유 차구나. 그럼
	  터보차저(Source Attribute)를 달고, 공기저항(Target Attribute)을 고려해서 최종 바퀴 출력을 250마력(Final Damage)으로 만들자" 라고
	  최종 계산을 완료하는 단계입니다.

   3. `ApplyGameplayEffectSpecToSelf`: 완성된 차를 도로(Target ASC)에 굴리는 것입니다.
   */
	
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
	return EffectContextHandle;
}
