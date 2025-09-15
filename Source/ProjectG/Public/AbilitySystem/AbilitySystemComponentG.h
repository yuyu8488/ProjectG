// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInputID.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentG.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTG_API UAbilitySystemComponentG : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityActorInfoSet();
	void InitAbility(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant); //AddCharacterAbilities
	
	void AbilityInputIDPressed(const EAbilityInputID& InputID);
	void AbilityInputIDReleased(const EAbilityInputID& InputID);
	void AbilityInputIDHeld(const EAbilityInputID& InputID);

	TMap<EAbilityInputID, int32>& GetAbilityComboCounter() {return AbilityComboCounter;}

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void TEST_SetAbilityComboCounterValue(EAbilityInputID AbilityInputID, int32 Value);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability.Combo")
	TMap<EAbilityInputID, int32> AbilityComboCounter;
};
