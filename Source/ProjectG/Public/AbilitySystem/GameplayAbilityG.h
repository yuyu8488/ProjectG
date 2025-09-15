// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInputID.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityG.generated.h"

struct FDamageEffectParams;

USTRUCT(BlueprintType)
struct FAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> AnimMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	bool bIsCombo = false;

	// 콤보 개수 3개면 3으로 설정 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo", meta=(EditCondition="bIsCombo"))
	int32 MaxComboStep = 1;
	
	// 콤보단계를 나타내는데, 0단계부터 시작(배열순서때문에)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo", meta=(EditCondition="bIsCombo"))
	int32 ComboStep = 0;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability.Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability.Damage")
	FScalableFloat DamageMultiplier = FScalableFloat(1.0f);
	
	UPROPERTY(EditDefaultsOnly, Category="Ability.Damage")
	FGameplayTag DamageType;
};

UCLASS()
class PROJECTG_API UGameplayAbilityG : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbilityG();
	
	UFUNCTION(BlueprintPure, Category="AbilitySystem")
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, const FGameplayTag InAbilityTag) const;

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	void ApplyDamageEffect(AActor* InTargetActor, const FGameplayTag InAbilityTag);

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	FAbilityData Info;
	
protected:
	// 몽타주 종료 콜백 함수
	UFUNCTION()
	virtual void OnComboSave();

	UFUNCTION()
	virtual void OnComboEnd();
	
	UFUNCTION()
	virtual void ResetCombo();
	

};
