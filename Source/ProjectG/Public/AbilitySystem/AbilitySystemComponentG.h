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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const;

	void OnAbilityActorInfoSet();
	void InitAbility(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant); 
	
	void AbilityInputIDPressed(const EAbilityInputID& InputID);
	void AbilityInputIDReleased(const EAbilityInputID& InputID);
	void AbilityInputIDHeld(const EAbilityInputID& InputID);

	TMap<EAbilityInputID, int32>& GetAbilityComboCounter() {return AbilityComboCounter;}

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SetAbilityComboCounterValue(EAbilityInputID AbilityInputID, int32 Value);
	
protected:
	// TMap에 커스텀 자료형이 들어갈 경우 추가 작업이 필요함 -> NetSerialize
	// Enum은 내부적으로 uint8이나 int32같은 정수로취급 문제x 


	//TODO: 음... FastArraySerializer 조사
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ability.Combo")
	TMap<EAbilityInputID, int32> AbilityComboCounter;
};
