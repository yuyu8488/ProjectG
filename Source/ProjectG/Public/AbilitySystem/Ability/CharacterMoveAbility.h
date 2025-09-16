// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbilityG.h"
#include "CharacterMoveAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTG_API UCharacterMoveAbility : public UGameplayAbilityG
{
	GENERATED_BODY()

public:
	UCharacterMoveAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
