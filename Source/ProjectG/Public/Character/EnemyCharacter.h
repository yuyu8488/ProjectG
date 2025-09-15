// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterG.h"
#include "Interface/EnemyInterface.h"
#include "EnemyCharacter.generated.h"


// Forward declarations
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyHealthChangedSignature, float, NewHealthValue);

UCLASS()
class PROJECTG_API AEnemyCharacter : public ACharacterG, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

	// Combat Interface
	virtual void UpdateTargets_Implementation() override;
	virtual void UpdatePrimaryTarget_Implementation() override;
	// ~ End Combat Interface
	
	// EnemyInterface	
	// ~ End EnemyInterface

	UPROPERTY(BlueprintAssignable)
	FOnEnemyHealthChangedSignature OnEnemyHealthChanged;

protected:
	virtual void InitAbilitySystem() override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;
	
};
