// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityInputID.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerG.generated.h"


UCLASS()
class PROJECTG_API APlayerControllerG : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerG();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void AbilityActionInputPressed(EAbilityInputID AbilityInputID);
	void AbilityActionInputReleased(EAbilityInputID AbilityInputID);
	void AbilityActionInputHeld(EAbilityInputID AbilityInputID);
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<class UInputConfig> InputConfig;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> LookAction;
};
