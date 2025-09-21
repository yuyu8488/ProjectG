// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInputID.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentG.generated.h"

USTRUCT(BlueprintType)
struct FAbilityComboInfo
{
	GENERATED_BODY()
public:
	FAbilityComboInfo() : AbilityInputID(EAbilityInputID::None), ComboStep(0) {};
	FAbilityComboInfo(const EAbilityInputID InAbilityInputID, const int32 InComboStep) : AbilityInputID(InAbilityInputID), ComboStep(InComboStep) {}

	bool operator==(const FAbilityComboInfo& Other) const
	{
		return AbilityInputID == Other.AbilityInputID;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ComboStep = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTG_API UAbilitySystemComponentG : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void OnAbilityActorInfoSet();
	void InitAbility(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant); 
	
	void AbilityInputIDPressed(const EAbilityInputID& InputID);
	void AbilityInputIDReleased(const EAbilityInputID& InputID);
	void AbilityInputIDHeld(const EAbilityInputID& InputID);

	TArray<FAbilityComboInfo>& GetAbilityComboCounter() {return AbilityComboCounter;}

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SetAbilityComboCounterValue(EAbilityInputID InAbilityInputID, int32 InComboStep);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Ability.Combo")
	TArray<FAbilityComboInfo> AbilityComboCounter;
};
