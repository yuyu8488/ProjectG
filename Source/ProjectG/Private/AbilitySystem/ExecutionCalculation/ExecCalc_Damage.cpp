// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculation/ExecCalc_Damage.h"

#include "GameplayTagG.h"
#include "AbilitySystem/AttributeSetG.h"
#include "AbilitySystem/GameplayAbilityG.h"

struct DamageStatics
{
	//Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	//Target
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetG, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetG, CriticalChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetG, CriticalDamage, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetG, Armor, Target, false);
	}
};

static DamageStatics GetDamageStatics()
{
	static DamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	/* RelevantAttributesToCapture 는 등록한 Attribute 값을 캡처(수집)해서 ExecutionParams에 담아줌 */
	RelevantAttributesToCapture.Add(GetDamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalDamageDef);
	
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 캐릭터의 공격력
	float AttackPowerValue = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluateParams, AttackPowerValue);

	// Ability의 공격력 퍼센트
	float DamagePercentValue = 1.f;
	DamagePercentValue = Spec.GetSetByCallerMagnitude(FGameplayTagG::Get().Data_GameplayAbility_DamagePercent, false, 1.f);

	// 적용할 데미지
	float ApplyDamage = 0.f;
	ApplyDamage = AttackPowerValue * (DamagePercentValue * 0.01);
	
	// 최종 데미지 OutExecutionOutput에 적용
	// AttributeSet의 PostGameplayEffectExecute 에서 IncomingDamage값을 Health에 적용.
	FGameplayModifierEvaluatedData ModifierEvaluatedData;
	ModifierEvaluatedData.Attribute = UAttributeSetG::GetInComingDamageAttribute();
	ModifierEvaluatedData.ModifierOp = EGameplayModOp::Override;
	ModifierEvaluatedData.Magnitude = ApplyDamage;
	OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
}
