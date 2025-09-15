// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbilityG.h"
#include "ProjectileAbility.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class PROJECTG_API UProjectileAbility : public UGameplayAbilityG
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	FName SpawnSocketName = "None";
};
 