// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/CharacterMoveAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagG.h"

UCharacterMoveAbility::UCharacterMoveAbility()
{
	// 즉시 실행되고 끝나서 인스턴스화 필요 x
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UCharacterMoveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTagG::Get().Character_State_AttackCancellable))
		{
			FGameplayTagContainer TagsToCancelAbility;
			TagsToCancelAbility.AddTag(FGameplayTagG::Get().Ability_BasicAttack);
			ASC->CancelAbilities(&TagsToCancelAbility);
		}
	}
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
