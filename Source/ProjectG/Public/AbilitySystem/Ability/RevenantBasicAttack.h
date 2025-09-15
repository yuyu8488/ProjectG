// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileAbility.h"
#include "RevenantBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTG_API URevenantBasicAttack : public UProjectileAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnComboSave() override;
	virtual void OnComboEnd() override;
	virtual void ResetCombo() override;
	
private:
	UFUNCTION()
	void OnFireEvent(FGameplayEventData InEventData);

	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;
};


