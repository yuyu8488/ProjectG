// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateG.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTG_API APlayerStateG : public APlayerState
{
	GENERATED_BODY()

public:
	APlayerStateG();
	class UAbilitySystemComponent* GetAbilitySystemComponent()  const {return AbilitySystemComponent;}
	class UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	
};
