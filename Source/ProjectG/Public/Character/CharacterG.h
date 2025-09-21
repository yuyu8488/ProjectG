// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "CharacterG.generated.h"

class UGameplayEffect;

UCLASS()
class PROJECTG_API ACharacterG : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ACharacterG();
	
	// AbilitySystem Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~End AbilitySystem Interface

	// Combat Interface
	virtual bool IsInCombat_Implementation() override;
	virtual void SetInCombat_Implementation(const bool InCombat) override;
	virtual void UpdateTargets_Implementation() override;
	virtual void UpdatePrimaryTarget_Implementation() override;
	virtual AActor* GetPrimaryTarget_Implementation() override;
	virtual void FacePrimaryTarget_Implementation() override;
	virtual FVector GetProjectileSocketLocation_Implementation(FName InSocketName) override;
	// ~ End Combat Interface
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitAbilitySystem();
	
	void InitCharacterAbility() const;
	void InitCharacterAttributes() const;
	
	UPROPERTY(VisibleAnywhere, Category="AbilitySystem")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category="AbilitySystem")
	TObjectPtr<class UAttributeSet> AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem.Ability")
	TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category="AbilitySystem.Attribute")
	TSubclassOf<UGameplayEffect> VitalAttributeEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat.Target", meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<AActor>> CombatTargets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat.Target", meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> PrimaryCombatTarget;
	
	// 적 개체를 인식했을때, 데미지를 줬을때. 데미지받을때(환경데미지 제외)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	bool bInCombat;
};
