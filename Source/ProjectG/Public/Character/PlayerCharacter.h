// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterG.h"
#include "PlayerCharacter.generated.h"

struct FGameplayTag;

UCLASS()
class PROJECTG_API APlayerCharacter : public ACharacterG
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// Combat Interface
	virtual void UpdateTargets_Implementation() override;
	virtual void UpdatePrimaryTarget_Implementation() override;
	// ~ End Combat Interface

	void SetLastInputDirection(const FVector& InDirection) {LastInputDirection = InDirection;}
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilitySystem() override;

private:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> SpringArm;
	
	// Move Input 추적
	UPROPERTY()
	FVector LastInputDirection = FVector::ZeroVector;

	
	//TODO: TargetSearch 구조체로 정리하기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search", meta=(AllowPrivateAccess="true"))
	float TargetSearchInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search", meta=(AllowPrivateAccess="true"))
	float TargetSearchRadius = 1000.f;

	// 타겟을 정할때 거리에 대한 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search", meta=(AllowPrivateAccess="true", UIMin="0.0", UIMax="1.0"))
	float TargetSearchDistanceWeight = 0.7f;
	
	// 타겟을 정할때 이미 Primary Target인경우 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search",  meta=(AllowPrivateAccess="true", UIMin="0.0", UIMax="1.0"))
	float TargetSearchPrimaryTargetWeight = 0.15f;

	// 타겟을 정할때 입력 방향과의 일치도에 대한 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search", meta=(AllowPrivateAccess="true", UIMin="0.0", UIMax="1.0"))
	float TargetSearchAlignmentWeight = 0.3f;

	// 타겟을 찾을때 최대 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat.Target.Search", meta=(AllowPrivateAccess="true", UIMin="0.0", UIMax="360.0"))
	float TargetSearchFieldOfView = 270.f;
};
