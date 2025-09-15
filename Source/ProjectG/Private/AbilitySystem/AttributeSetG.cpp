// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSetG.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTagG.h"

UAttributeSetG::UAttributeSetG()
{
	const FGameplayTagG& GameplayTagG = FGameplayTagG::Get();
	//TagToAttribute.Add(GameplayTagG);
}

void UAttributeSetG::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/** Vital */
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, MaxMana, COND_None, REPNOTIFY_Always);

	/** Primary */
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, Vitality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetG, CriticalDamage, COND_None, REPNOTIFY_Always);
}

void UAttributeSetG::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/* GameplayEffect 등에 의해 어떤 어트리뷰트의 값이 변경되려고 할 때, 그 값이 실제로 적용되기 바로 직전에
	PreAttributeChange 함수가 호출됩니다. 이 함수는 새로운 값이 적절한지 검사하고, 필요하다면 수정할 수있는 마지막 기회를 제공합니다.
	 */	
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		const float TMaxHealth = GetMaxHealth();
		NewValue = FMath::Clamp(NewValue, 0.0f, TMaxHealth);
	}
	if (Attribute == GetManaAttribute())
	{
		const float TMaxMana = GetMaxMana();
		NewValue = FMath::Clamp(NewValue, 0.0f, TMaxMana);
	}
}

void UAttributeSetG::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
		
	
}

void UAttributeSetG::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	// GameplayEffect가 성공적으로 실행되고 Attribute 값의 변경이 완료되면, 이 함수가 자동으로 호출된다.
	// GameAbility의 특정 능력이 공격에 성공하고 GameplayEffect(GE_Damage)의 적용중에 ExecCalc_Damage에서 계산된 값이 IncomingDamage(Attribute) 값을 변경
	// 변경된 IncomingDamage 값을 Health에 반영.
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetInComingDamageAttribute())
	{
		const float LocalIncomingDamage = GetInComingDamage();
		SetInComingDamage(0.f);

		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		}
	}
}

void UAttributeSetG::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, Health, OldHealth);
}

void UAttributeSetG::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, MaxHealth, OldMaxHealth);
}

void UAttributeSetG::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, Mana, OldMana);
}

void UAttributeSetG::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, MaxMana, OldMaxMana);
}

void UAttributeSetG::OnRep_Vitality(const FGameplayAttributeData& OldVitality) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, Vitality, OldVitality);
}

void UAttributeSetG::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, AttackPower, OldAttackPower);
}

void UAttributeSetG::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, Armor, OldArmor);
}

void UAttributeSetG::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, CriticalChance, OldCriticalChance);
}

void UAttributeSetG::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetG, CriticalDamage, OldCriticalDamage);
}

